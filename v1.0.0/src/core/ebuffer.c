/*
  +----------------------------------------------------------------------+
  | libeasy                                                              |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | yiming_6weijun@163.com so we can mail you a copy immediately.        |
  +----------------------------------------------------------------------+
  | Author: Weijun Lu  <yiming_6weijun@163.com>                          |
  +----------------------------------------------------------------------+
*/
#include "ebuffer.h"

static int buffer_enlarge(buffer_t *buffer, int dst_size);

int buffer_eof(buffer_t *buffer)
{
    if (NULL == buffer) {
        log_error("buffer_eof: buffer == NULL.");
        return 0;
    }

    if (buffer->pos >= buffer->last) {
        return 1;
    }
    else {
        return 0;
    }
}

int buffer_read(buffer_t *buffer, char *buf, int buf_size)
{
    int read_len = 0;

    if (NULL == buffer || NULL == buf || buf_size <= 0) {
        log_error("buffer_read: NULL == buffer || NULL == buf || buf_size <= 0.");
        return -1;
    }

    read_len = buf_size;

    if (buffer->last - buffer->pos < read_len) {
        read_len = buffer->last - buffer->pos; 
    }

    memcpy(buf, buffer->pos, read_len);
    buffer->pos += read_len;

    return read_len;
}

int buffer_read_line(buffer_t *buffer, char *buf, int buf_size)
{
    int   read_len  = 0; 
    char *start_pos = buffer->pos;

    if (NULL == buffer || NULL == buf || buf_size <= 0) {
        log_error("buffer_read_line: NULL == buffer || NULL == buf || buf_size <= 0.");
        return -1;
    }
    
    while (buffer->pos < buffer->last) {
        if (read_len >= buf_size) {
            buffer->pos = start_pos;
            log_error("buffer_read_line: buf is not enough.");
            return -1;
        }
        
        if (*buffer->pos == '\r') {
            memcpy(buf, start_pos, read_len);
            buf[read_len] = 0;
            buffer->pos += 2;
            break;
        }

        if (*buffer->pos == '\n') {
            memcpy(buf, start_pos, read_len);
            buf[read_len] = 0;
            buffer->pos++;
            break;
        }

        read_len++;
        buffer->pos++;
    }

    return read_len;
}

int buffer_write(buffer_t *buffer, char *buf, int size)
{
    /* 如果buffer需要扩容 */
    if (buffer->end - buffer->last < size) {
        if (!buffer_enlarge(buffer, buffer->end - buffer->start + size)) {
            log_error("buffer_enlarge failed.");
            return -1;
        }
    }

    memcpy(buffer->last, buf, size);
    buffer->last += size;

    return size;
}

/*
 * 1 -> 扩容成功
 * 0 -> 扩容失败
 */
static int buffer_enlarge(buffer_t *buffer, int dst_size)
{
    char *ptr = NULL;
    int   pos_off;
    int   last_off;
    int   end_off;

    if (buffer->end - buffer->start >= dst_size) {
        log_error("buffer_enlarge: buffer->end - buffer->start >= dst_size.");
        return 0;
    }

    pos_off  = buffer->pos  - buffer->start;
    last_off = buffer->last - buffer->start;
    end_off  = buffer->end  - buffer->start;

    if (buffer->pool) {
        ptr = mem_pool_calloc(buffer->pool, dst_size); 
    }
    else {
        ptr = malloc(dst_size);
    }
    
    if (NULL == ptr) {
        log_error("buffer_enlarge: ptr == NULL."); 
        return 0;
    }

    if (buffer->last - buffer->start > 0) {
        memcpy(ptr, buffer->start, buffer->last - buffer->start);
    }
    buffer->start = ptr;
    buffer->pos   = ptr + pos_off;
    buffer->last  = ptr + last_off;
    buffer->end   = ptr + dst_size;

    return 1;
}
