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
*  hashmap.h
*
*  generic hash table - stores and maps key + data pairs
*
*  Author: Marc Norton
*
*/

#ifndef _HASHMAP_
#define _HASHMAP_

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash_fcn.h"

/*
*   ERROR DEFINES
*/
#define HASHMAP_NOMEM    -2
#define HASHMAP_ERR      -1
#define HASHMAP_OK        0
#define HASHMAP_INTABLE   1

/* 
*  Flags for ghash_new: userkeys 
*/
#define GH_COPYKEYS 0
#define GH_USERKEYS 1

/*
*   Generic HASH NODE
*/
typedef struct _hashmap_node_t{
  struct _hashmap_node_t *next, *prev;

  void *key;   /* Copy of, or Pointer to, the Users key */
  void *data;  /* Pointer to the users data, this is never copied! */
     
}hashmap_node_t;

/*
*    Generic HASH table
*/
typedef struct _hashmap_t{
  hash_fcn_t  *hash_fcn;
  int          keysize;   /* bytes in key, if < 0 -> keys are strings */
  int          userkey;   /* user owns the key */

  hashmap_node_t **table;  /* array of node ptr's */
  int              nrows;  /* # rows int the hash table use a prime number 211, 9871 */
  unsigned         count;  /* total # nodes in table */

  void (*userfree)(void *);  

  int             crow;    // findfirst/next row in table
  hashmap_node_t *cnode; // findfirst/next node ptr

  int splay;
}hashmap_t;


/*
*   HASH PROTOTYPES
*/
hashmap_t * hashmap_new(int nrows, int keysize, int userkeys, void (*userfree)(void*p));
void      hashmap_delete(hashmap_t *h);
int       hashmap_add(hashmap_t *h, void *key, void *data);
int       hashmap_remove(hashmap_t *h, void *key);
int       hashmap_count(hashmap_t *h);
void    * hashmap_find(hashmap_t *h, void *key );
int       hashmap_find2(hashmap_t *h, void *, void **);
hashmap_node_t * hashmap_findfirst(hashmap_t *h);
hashmap_node_t * hashmap_findnext (hashmap_t *h);
void hashmap_splaymode(hashmap_t *t, int n);

int hashmap_set_keyops(hashmap_t *h ,
                        unsigned (*hash_fcn)(hash_fcn_t *p,
                                             unsigned char *d,
                                             int n),
                        int (*keycmp_fcn)(const void *s1,
                                          const void *s2,
                                          size_t n));


#endif

