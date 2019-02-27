#ifndef _MY_MALLOC_H_
#define _MY_MALLOC_H_
#include <stdbool.h>
struct _myblock{
  size_t size;// record block's size                                            
  struct _myblock* next;//next block                                            
  struct _myblock* prev;//previous block 
  bool free;//when the block is free,true;otherwise,false
};

typedef struct _myblock *myblock;

void *ff_malloc (size_t size);
void ff_free (void *ptr);
void *bf_malloc (size_t size);
void bf_free (void *ptr);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
#endif
