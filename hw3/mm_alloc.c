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
size_t align4(size_t *x){ 
  return (((((*x)-1)>>2)<<2)+4);
}

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

s_block_ptr fusion(s_block_ptr b){
  if(b->next && b->next->free){
    b->size += BLOCK_SIZE + b->next->size;
		b->next = b->next->next;
		if (b->next)
			b->next->prev = b;
  }
	return b;
}

s_block_ptr get_block(void *p){
  char *tmp;
	tmp = p;
	tmp -= BLOCK_SIZE;
	p = tmp;
	return p;
}

int valid_addr(void *p){
  if(base != NULL){
		if(p > base && p < sbrk(0)){
			return p == (get_block (p))->ptr;
		}
	}
	return 0;
}

void* mm_malloc(size_t size)
{
/*#ifdef MM_USE_STUBS
  return calloc(1, size);
#else
#error Not implemented.
#endif */
	s_block_ptr block, last;
	//size_t s = align4(&size);
	size_t s = size;
	if(base != NULL){
    last = base;

		/*SEARCH NEXT BLOCK*/
    block = base;
		while (block != NULL && !(block->free && block->size >= s)){
			 last = block;
			 block = block->next;
		}
		//block = next;
     
		if(block != NULL){
      if ((block->size - s) >= ( BLOCK_SIZE + 4))
				split_block(block,s);
			block->free = 0;
		} else{
      block = extend_heap(last,s);
			if (!block)
				return NULL;
		}
	} else{
    block = extend_heap(NULL, s);
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
		size_t s, i;
		s_block_ptr b, new;
    void *newp;
		if(!ptr)
			return mm_malloc(size);
		if(valid_addr(ptr)){
      s = align4(&size);
			b = get_block(ptr);
			if(b->size >= s){
        if (b->size - s >= ( BLOCK_SIZE + 4))
					split_block (b,s);
			} 
			else
			{
        if (b->next && b->next->free && (b->size + BLOCK_SIZE + b->next ->size) >= s){
         fusion(b);
				 if (b->size - s >= (BLOCK_SIZE + 4))
					 split_block (b,s);
			  } 
				else {
          newp = mm_malloc(size);
				  if (!newp){
					  return NULL;
					}
				  new = get_block(newp);

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
  if(valid_addr(ptr)){
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

