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

s_block_ptr get_block(void *p){
  char *tmp;
	tmp = p;
	tmp -= BLOCK_SIZE;
	p = tmp;
	return p;
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
     
		if(block != NULL){
      if ((block->size - size) >= (BLOCK_SIZE + 4))
				split_block(block,size);
			block->free = 0;
		} else{
      block = extend_heap(last,size);
			if (!block)
				return NULL;
		}
	} else{
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
		size_t i;
		s_block_ptr b, new;
    void *newp;
		if(!ptr)
			return mm_malloc(size);
		if(ptr){
			//b = get_block(ptr);
			b = ptr - BLOCK_SIZE;
			if(b->size >= size){
        if (b->size - size >= ( BLOCK_SIZE + 4))
					split_block (b,size);
			} 
			else
			{
        if (b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size) >= size){
         fusion(b);
				 if (b->size - size >= (BLOCK_SIZE + 4))
					 split_block (b,size);
			  } 
				else {
          newp = mm_malloc(size);
				  if (!newp){
					  return NULL;
					}
				  //new = get_block(newp);
					new = newp - BLOCK_SIZE;

					/*COPYING DATA*/
					int *bcopy = b->ptr;
					int *newcopy = new->ptr;
				  for (i=0; i*4< b->size && i*4 < new->size; i++){
						newcopy[i] = bcopy[i];
					}
					//memcpy(b, new, sizeof(struct s_block*));
				  mm_free(ptr);
				  return newp;
			  }
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
    block = get_block(ptr);
		block->free = 1;
    if (block->prev && block->prev->free)
			block = fusion(block->prev);

    if (block->next)
			fusion(block);
		else{
      if(block->prev)
				block->prev->next = NULL;
			else
				base = NULL;
			brk(block);
		}
	}
}

