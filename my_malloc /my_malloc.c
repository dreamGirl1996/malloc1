#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "my_malloc.h"
#include<assert.h>

void *start=NULL;
unsigned long heap_size=0;
#define block_size sizeof(struct _myblock)


unsigned long free_segment=0;




/*separate the space into used and unused to save space and connect then with  heap*/
void separate(myblock m,size_t size){ 
  myblock new;
  new=(myblock)((char*)(m+1)+size);//data
  assert(m!=NULL);
  new->size=m->size-size-block_size;
  new->free=true;
  new->next=m->next;
  if(new->next!=NULL){
    new->next->prev=new;
  }
  new->prev=m;
  m->size=size;
  m->next=new;

}


myblock new_heap(myblock end, size_t size){
  myblock m;
  m=sbrk(0);

  if(sbrk(block_size+size)!=(void*)-1){
    m->size=size;
    m->next=NULL;
    heap_size=(long)m-(long)start;//record heap size between current address and start
  }else{
    return NULL;
  }
  if(end){//put m at the end
    m->prev=end;
    end->next=m;
  }
  m->free=false;
  return m;
}


void *ff_malloc(size_t size){
  //printf("========\n");
  myblock m;
  if(start==NULL){// when start is NULL,new heap
    m=new_heap(NULL,size);
    if(m==NULL){
      perror("allocation failure");
      return NULL;
    }
    start=m;
  }else{
    myblock curr=start;
    myblock last=start;
    while(curr){
      if(curr->free&&(curr->size>=size+block_size)){
	//last=curr;
	break;//find the address where  it is  free and enough size
      }
      last=curr;
      //  printf("kkkk\n");
      curr=curr->next;
    }
    // printf("aaa\n");
    if(curr!=NULL){
      //printf("bbb\n");
      separate(curr,size);
      curr->free=false;
      m=curr;
    }else{
      // printf("ccc\n");
      last=new_heap(last,size);//malloc new space at the end of  heap
      if(last==NULL){
	return NULL;
      }
      m=last;
    }
  }
  return m+1;
  }




void *bf_malloc(size_t size){
  myblock temp;
  if(start==NULL){
    temp=new_heap(NULL,size);
    if(temp==NULL){
      return NULL;
    }
    start=temp;

  }else{
    myblock curr=start;
    myblock min=start;
    size_t min_size = 0;
    myblock last = start;
    while(curr){
      if((curr->free)&&(curr->size>=size+block_size)){//if current is free and size is bigger than assigned size
	if((min_size>curr->size)||min_size==0){
	  min_size = curr->size;
	  min=curr;
	}
      }
      last = curr;
      curr=curr->next;
    }
    if(min_size!=0){
      if(min_size>size+block_size){
      separate(min,size);
      }
      min->free=false;
    }else{
      min=new_heap(last,size);
      if(min==NULL){
	return NULL;
      }
    }
    temp=min;
  }
  return temp+1;
  }


void merge(myblock m){
  if(m->next&&m->next->free==true){//merge the block next to m if this block is free
    m->size=m->size+m->next->size+block_size;
    m->next=m->next->next;
    if(m->next){
    m->next->prev=m;
    }
  }
}


void ff_free(void *ptr){
  myblock  m;
   m=(myblock )ptr-1;
  m->free=true;
  if(m->next){
    merge(m);
  }
  if(m->prev&&m->prev->free==true){
    merge(m->prev);
  }
  if(m->next==NULL){ 
        if(m->prev!=NULL){
      m->prev->next=NULL;
      }
    m->free=true;
    }
  // printf("*****\n");
  }


void bf_free(void *ptr){
  ff_free(ptr);
}

unsigned long get_data_segment_size(){return heap_size;} //in bytes
unsigned long get_data_segment_free_space_size(){
  myblock m;
  m=start;
  while(m){
    if(m->free==true){
      free_segment+=m->size;
    }
    m=m->next;
  }
  return free_segment;

} //in bytes

