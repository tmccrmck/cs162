/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* Your final implementation should comment out this macro. */
/* #define MM_USE_STUBS */

/*base of heap*/
void *base = NULL;

void split_block (s_block_ptr block, size_t s){
  s_block_ptr new;
	new = (s_block_ptr)(block->data + s);
	new->size = block->size - s - BLOCK_SIZE ;
	new->next = block->next;
	new->prev = block;
  new->free = 1;
	new->ptr = new->data;
	block->size = s;
}

s_block_ptr extend_heap (s_block_ptr last, size_t s){
  s_block_ptr block;
	int newEnd;
	block = sbrk(0);
	newEnd = (int) sbrk(BLOCK_SIZE + s);
	if (newEnd < 0)
		return NULL;
	block->size = s;
	block->next = NULL;
	block->prev = last;
	block->ptr = block->data;
	block->free = 0;
	return block;
}

s_block_ptr fusion(s_block_ptr block){
  if(block->next->free == 1){
    block->size += BLOCK_SIZE + block->next->size;
		block->next = block->next->next;
  }
	return block;
}

void* mm_malloc(size_t size)
{
/*#ifdef MM_USE_STUBS
  return calloc(1, size);
#else
#error Not implemented.
#endif */
	s_block_ptr block, last;
	if(base != NULL){
    last = base;

		/*SEARCH NEXT BLOCK*/
    block = base;
		while (block != NULL && !(block->free && block->size >= size)){
			 last = block;
			 block = block->next;
		}
     
    block = extend_heap(last,size);
	  if (!block)
			return NULL;
	} else{
		/* ON FIRST CALL */
    block = extend_heap(NULL, size);
		if(!block)
			return NULL;
		base = block;
	}
	return block->data;
}

void* mm_realloc(void* ptr, size_t size)
{
/*#ifdef MM_USE_STUBS
    return realloc(ptr, size);
#else
#error Not implemented.
#endif*/
		s_block_ptr block, new;
    void *newp;
		if(ptr){
			block = ptr - BLOCK_SIZE;
      if (block->next && block->next->free /*&& (block->size + BLOCK_SIZE + block->next->size) >= size*/){
         fusion(block);
			 } 
			else {
          newp = mm_malloc(size);
				  if (!newp){
					  return NULL;
					}
					new = newp - BLOCK_SIZE;

					/*COPYING DATA*/
					memcpy(new->data, block->data, block->size);
				  mm_free(ptr);
				  return newp;
			}
		  return ptr;
		}
  return NULL;
}

void mm_free(void* ptr)
{
/*#ifdef MM_USE_STUBS
    free(ptr);
#else
#error Not implemented.
#endif*/
  s_block_ptr block;
  if(ptr){
    block = ptr - BLOCK_SIZE;
		block->free = 1;

    if (block->next != NULL)
			fusion(block);
		
		brk(block);
	}
}

