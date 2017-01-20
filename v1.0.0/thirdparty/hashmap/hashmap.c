/****************************************************************************
 *
 * Copyright (C) 2003-2012 Sourcefire, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as
 * published by the Free Software Foundation.  You may not use, modify or
 * distribute this program under any other version of the GNU General
 * Public License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 ****************************************************************************/

/*
*
*  hashmap.c
*
*  Generic hash table library.
*
*  This hash table maps unique keys to void data pointers.
*
*  Features:
*    1) Keys may be ascii strings of variable size, or
*       fixed length (per table) binary byte sequences.  This
*       allows use as a Mapping for String+Data pairs, or a
*       generic hashing.
*    2) User can allocate keys, or pass copies and we can
*       allocate space and save keys.
*    3) User can pass a free function to free up user data
*       when the table is deleted.
*    4) Table rows sizes can be automatically adjusted to
*       the nearest prime number size.
*
*  6/10/03 - man - Upgraded the hash function to a Hardened hash function,
*      it has no predictable cycles, and each hash table gets a different
*      randomized hashing function. So even with the source code, you cannot predict
*      anything with this function.  If an attacker can setup a feedback
*      loop he might gain some knowledge of how to muck with us, but even in that case
*      his odds are astronomically skinny.  This is actually the same problem as solved
*      early on with hashing functions where degenerate data with close keys could
*      produce very long bucket chains.
*
*  8/31/06 - man - Added prime tables to speed up prime number lookup.
*
* Author: Marc Norton
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashmap.h"
#include "prime_table.h"

/*
*  Private Malloc
*/
static
void * s_alloc( int n )
{
     return calloc( 1,n );
}

/*
*  Private Free
*/
static
void s_free( void * p )
{
   if( p )free( p );
}

/*
*
*    Create a new hash table
*
*    nrows    : number of rows in hash table, primes are best.
*               > 0  => we use the nearest prime internally
*               < 0  => we use the magnitude as nrows.
*    keysize  : > 0 => bytes in each key, keys are binary bytes,
*               all keys are the same size.
*               ==0 => keys are strings and are null terminated,
*               allowing random key lengths.
*    userkeys : > 0 => indicates user owns the key data
*               and we should not allocate or free space for it,
*               nor should we attempt to free the user key. We just
*               save the pointer to the key.
*               ==0 => we should copy the keys and manage them internally
*    userfree : routine to free users data, null if we should not
*               free user data in hashmap_delete(). The routine
*               should be of the form 'void userfree(void * userdata)',
*               'free' works for simple allocations.
*/
hashmap_t * hashmap_new(int nrows, int keysize, int userkeys, void (*userfree)(void*p))
{
   int    i;
   hashmap_t *h;

   if( nrows > 0 ){ /* make sure we have a prime number */
      nrows = nearest_prime( nrows );
   }
   else{   /* use the magnitude or nrows as is */
      nrows = -nrows;
   }

   h = (hashmap_t*)s_alloc( sizeof(hashmap_t) );
   if( !h )
	   return 0;

   memset( h, 0, sizeof(hashmap_t) );

   h->hash_fcn = hash_fcn_new( nrows );
   if( !h->hash_fcn )
   {
       free(h);
	   return 0;
   }

   h->table = (hashmap_node_t**) s_alloc( sizeof(hashmap_node_t*) * nrows );
   if( !h->table )
   {
       free(h->hash_fcn);
       free(h);
	   return 0;
   }

   for( i=0; i<nrows; i++ )
   {
      h->table[i] = 0;
   }

   h->userkey = userkeys;

   h->keysize = keysize;

   h->nrows = nrows;

   h->count = 0;

   h->userfree = userfree;

   h->crow = 0; // findfirst/next current row

   h->cnode = 0; // findfirst/next current node ptr

   return h;
}

/*
*  Set Splay mode : Splays nodes to front of list on each access
*/
void hashmap_splaymode( hashmap_t * t, int n )
{
   t->splay = n;
}

/*
*  Delete the hash Table
*
*  free key's, free node's, and free the users data, if they
*  supply a free function
*/
void hashmap_delete( hashmap_t * h )
{
  int            i;
  hashmap_node_t * node, * onode;

  if( !h ) return;

  hash_fcn_free( h->hash_fcn );

  if( h->table )
  {
    for(i=0;i<h->nrows;i++)
    {
      for( node=h->table[i]; node;  )
      {
        onode = node;
        node  = node->next;

        if( !h->userkey && onode->key )
            s_free( onode->key );

        if( h->userfree && onode->data )
            h->userfree( onode->data ); /* free users data, with users function */

        s_free( onode );
      }
    }
    s_free( h->table );
    h->table = 0;
  }

  s_free( h );
}

/*
*  Get the # of Nodes in HASH the table
*/
int hashmap_count( hashmap_t * t )
{
  return t->count;
}



/*
*  Add a key + data pair
*  ---------------------
*
*  key + data should both be non-zero, although data can be zero
*
*  t    - hash table
*  key  - users key data (should be unique in this table)
*         may be ascii strings or fixed size binary keys
*  data - users data pointer
*
*  returns  HASHMAP_NOMEM: alloc error
*           HASHMAP_INTABLE : key already in table (t->cnode points to the node)
*           HASHMAP_OK: added a node for this key + data pair
*
*  Notes:
*  If the key node already exists, then t->cnode points to it on return,
*  this allows you to do something with the node - like add the data to a
*  linked list of data items held by the node, or track a counter, or whatever.
*
*/
int hashmap_add( hashmap_t * t, void * key, void * data )
{
    unsigned    hashkey;
	int         klen;
    int         index;
    hashmap_node_t  *hnode;

    if (t == NULL)
        return HASHMAP_ERR;

    /*
    *   Get proper Key Size
    */
    if( t->keysize > 0  )
    {
        klen = t->keysize;
    }
    else
    {
	     /* need the null byte for strcmp() in hashmap_find() */
        klen = strlen( (char*)key ) + 1;
    }

    hashkey = t->hash_fcn->hash_fcn(  t->hash_fcn, (unsigned char*) key, klen );

    index = hashkey % t->nrows;

    /*
    *  Uniqueness:
    *  Check 1st to see if the key is already in the table
    *  Just bail if it is.
    */
    for( hnode=t->table[index]; hnode; hnode=hnode->next )
    {
       if( t->keysize > 0 )
       {
          if( !t->hash_fcn->keycmp_fcn(hnode->key,key,klen) )
          {
              t->cnode = hnode; /* save pointer to the node */
              return HASHMAP_INTABLE; /* found it */
          }
       }
       else
       {
         if( !strcmp((const char *)hnode->key,(const char*)key) )
         {
             t->cnode = hnode; /* save pointer to the node */
             return HASHMAP_INTABLE; /* found it */
         }
       }
    }

    /*
    *  Create new node
    */
    hnode = (hashmap_node_t*)s_alloc(sizeof(hashmap_node_t));
    if( !hnode )
         return HASHMAP_NOMEM;

    /* Add the Key */
    if( t->userkey )
    {
      /* Use the Users key */
      hnode->key = key;
    }
    else
    {
      /* Create new key */
      hnode->key = s_alloc( klen );
      if( !hnode->key )
      {
           free(hnode);
           return HASHMAP_NOMEM;
      }

      /* Copy key  */
      memcpy(hnode->key,key,klen);
    }

    /* Add The Node */
    if( t->table[index] ) /* add the node to the existing list */
    {
        hnode->prev = 0;  // insert node as head node
        hnode->next=t->table[index];
        hnode->data=data;
        t->table[index]->prev = hnode;
        t->table[index] = hnode;
    }
    else /* 1st node in this list */
    {
        hnode->prev=0;
        hnode->next=0;
        hnode->data=data;
        t->table[index] = hnode;
    }

    t->count++;

    return HASHMAP_OK;
}

/*
*  move a node to the front of the list
*/
static void movetofront( hashmap_t *t , int index, hashmap_node_t * n )
{
    if( t->table[index] != n ) // if not at front of list already...
    {
      /* Unlink the node */
      if( n->prev ) n->prev->next = n->next;
      if( n->next ) n->next->prev = n->prev;

      /* Link at front of list */
      n->prev=0;
      n->next=t->table[index];
      t->table[index]->prev=n;
    }
}

/*
*  Find a Node based on the key, return users data.
*/
static hashmap_node_t * hashmap_find_node( hashmap_t * t, void * key)
{
    unsigned    hashkey;
    int         index, klen;
    hashmap_node_t  *hnode;

    if( t->keysize  )
    {
	klen = t->keysize;
    }
    else
    {
	klen = strlen( (char*) key ) + 1;
    }

    hashkey = t->hash_fcn->hash_fcn(  t->hash_fcn, (unsigned char*) key, klen );

    index = hashkey % t->nrows;

    for( hnode=t->table[index]; hnode; hnode=hnode->next )
    {
        if( t->keysize == 0 )
        {
           if( !strcmp((char*)hnode->key,(char*)key) )
           {
               if( t->splay  > 0 )
                   movetofront(t,index,hnode);

               return hnode;
           }
        }
        else
        {
           if( !t->hash_fcn->keycmp_fcn(hnode->key,key,t->keysize) )
           {
               if( t->splay  > 0 )
                   movetofront(t,index,hnode);

               return hnode;
           }
        }
    }

   return NULL;
}

/*
*  Find a Node based on the key, return users data.
*/
void * hashmap_find( hashmap_t * t, void * key)
{
    hashmap_node_t * hnode;

    if (t == NULL)
        return NULL;

    hnode = hashmap_find_node( t, key );

    if( hnode ) return hnode->data;

    return NULL;
}

/* Returns whether or not the there is an entry in the table with key
 * Sets argument data to data in hash node which could be NULL.
 * This function is used to both make sure there is an entry in the
 * table and get potential data associated with entry */
int hashmap_find2(hashmap_t *t, void *key, void **data)
{
    hashmap_node_t * hnode;

    if (t == NULL)
        return 0;

    hnode = hashmap_find_node(t, key);

    if (hnode != NULL)
    {
        *data = hnode->data;
        return 1;
    }

    return 0;
}

/*
*  Unlink and free the node
*/
static int hashmap_free_node( hashmap_t * t, unsigned index, hashmap_node_t * hnode )
{
    if( !t->userkey && hnode->key )
        s_free( hnode->key );
    hnode->key = 0;

    if( t->userfree && hnode->data )
        t->userfree( hnode->data ); /* free users data, with users function */

    if( hnode->prev )  // not the 1st node
    {
          hnode->prev->next = hnode->next;
          if( hnode->next ) hnode->next->prev = hnode->prev;
    }
    else if( t->table[index] )  // 1st node
    {
           t->table[index] = t->table[index]->next;
           if( t->table[index] )t->table[index]->prev = 0;
    }

    s_free( hnode );

    t->count--;

    return HASHMAP_OK;
}

/*
*  Remove a Key/Data Pair from the table - find it, unlink it, and free the memory for it.
*
*  returns : 0 - OK
*           -1 - node not found
*/
int hashmap_remove( hashmap_t * t, void * key)
{
    hashmap_node_t * hnode;
    int klen;
    unsigned hashkey, index;

    if( t->keysize > 0 )
    {
       klen = t->keysize;
    }
    else
    {
       klen = strlen((char*)key) + 1;
    }

    hashkey = t->hash_fcn->hash_fcn(  t->hash_fcn, (unsigned char*) key, klen );

    index = hashkey % t->nrows;

    for( hnode=t->table[index]; hnode; hnode=hnode->next )
    {
       if( t->keysize > 0 )
       {
         if( !t->hash_fcn->keycmp_fcn(hnode->key,key,klen) )
         {
             return hashmap_free_node( t, index, hnode );
         }
       }
       else
       {
         if( !strcmp((const char *)hnode->key,(const char*)key) )
         {
             return hashmap_free_node( t, index, hnode );
         }
       }
    }

   return HASHMAP_ERR;
}


/* Internal use only */
static void hashmap_next( hashmap_t * t )
{
    if( !t->cnode )
        return ;

    /* Next node in current node list */
    t->cnode = t->cnode->next;
    if( t->cnode )
    {
        return;
    }

    /* Next row */
    /* Get 1st node in next non-emtoy row/node list */
    for( t->crow++; t->crow < t->nrows; t->crow++ )
    {
       t->cnode = t->table[ t->crow ];
       if( t->cnode )
       {
           return;
       }
    }
}
/*
*   Get First Hash Table Node
*/
hashmap_node_t * hashmap_findfirst( hashmap_t * t )
{
    hashmap_node_t * n;

    /* Start with 1st row */
    for( t->crow=0; t->crow < t->nrows; t->crow++ )
    {
       /* Get 1st Non-Null node in row list */
       t->cnode = t->table[ t->crow ];

       if( t->cnode )
       {
         n = t->cnode;

         hashmap_next( t ); // load t->cnode with the next entry

         return n;
       }
    }
  return NULL;
}

/*
*   Get Next Hash Table Node
*/
hashmap_node_t * hashmap_findnext( hashmap_t * t )
{
    hashmap_node_t * n;

    n = t->cnode;

    if( !n ) /* Done, no more entries */
    {
        return NULL;
    }

    /*
       Preload next node into current node
    */
    hashmap_next( t );

    return  n;
}


/**
 * Make hash_fcn use a separate set of operators for the backend.
 *
 * @param h hash_fcn ptr
 * @param hash_fcn user specified hash function
 * @param keycmp_fcn user specified key comparisoin function
 */

int hashmap_set_keyops( hashmap_t *h ,
                        unsigned (*hash_fcn)( hash_fcn_t * p,
                                              unsigned char *d,
                                              int n),
                        int (*keycmp_fcn)( const void *s1,
                                           const void *s2,
                                           size_t n))
{
    if(h && hash_fcn && keycmp_fcn)
    {
        return hash_fcn_set_keyops(h->hash_fcn, hash_fcn, keycmp_fcn);
    }

    return -1;
}
