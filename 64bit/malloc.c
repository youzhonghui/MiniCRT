//malloc.c
#include "minicrt.h"

typedef struct _heap_header
{
	enum{
		HEAP_BLOCK_FREE	= 0XABABABAB,
		HEAP_BLOCK_USED	= 0XCDCDCDCD
	}type;
	unsigned long size;
	struct  _heap_header* next;
	struct  _heap_header* prev; 
}heap_header;

#define ADDR_ADD(a,o) (((char*)(a)) + o)
#define HEADER_SIZE	(sizeof(heap_header))

heap_header* list_head	= NULL;		///static

void free(void* ptr)
{
	heap_header* header	= (heap_header*)ADDR_ADD(ptr,-HEADER_SIZE);
	if (header->type != HEAP_BLOCK_USED)
	{
		return ;
	}

	header->type	= HEAP_BLOCK_FREE;
	if (header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE)
	{
		//MERGE
		header->prev->next = header->next;
		if(header->next != NULL)
			header->next->prev = header->prev;
		header->prev->size += header->size;

		header	= header->prev;
	}

	if (header->next != NULL && header->next->type == HEAP_BLOCK_FREE)
	{
		//MERGE
		header->size += header->next->size;
		header->next = header->next->next;
	}
}

void* malloc(unsigned long size)
{
	heap_header *header;
	if (size == 0)
	{
		return NULL;
	}

	header	= list_head;
	while(header != 0)
	{
		if (header->type == HEAP_BLOCK_USED)
		{
			header = header->next;
			continue;
		}

		if ((header->size > size +HEADER_SIZE)
			&&(header->size <= size + HEADER_SIZE *2))
		{
			header->type = HEAP_BLOCK_USED;
		}

		if (header->size > size + HEADER_SIZE*2)
		{
			//split
			heap_header* next = (heap_header*)ADDR_ADD(header,size + HEADER_SIZE);
			next->prev	= header;
			next->next	= header->next;
			next->type	= HEAP_BLOCK_FREE;
			next->size	= header->size -(size + HEADER_SIZE);
			header->next	= next;
			header->size	= size +HEADER_SIZE;
			header->type	= HEAP_BLOCK_USED;
			return	ADDR_ADD(header,HEADER_SIZE);
		}
		header = header->next;
	}

	return NULL;
}

#ifndef WIN32
//Linux brk system call
static long brk(void* end_data_segment)
{
	long ret	= 0;
	//brk system call number:45
	//long //usr/include/asm-i386/unistd.h
	//#define __NR_brk 45
	asm( "movq $45, %%rax   \n\t"
		 "movq %1, %%rbx  \n\t"
		 "int $0x80		\n\t"
		 "movq %%rax,%0  \n\t"
		 : "=r"(ret):"m"(end_data_segment));
}
#endif

#ifdef WIN32
#include <Windows.h>
#endif

long mini_crt_init_heap()
{
	void *base	= NULL;
	heap_header *header	= NULL;
	//32MB heap size
	unsigned long heap_size	= 1024*1024*32;

#ifdef WIN32
	base = VirtualAlloc(0,heap_size,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
	if (base == NULL)
	{
		return 0;
	}

#else
	base	= (void*)brk(0);
	void* end = ADDR_ADD(base,heap_size);
	end		= (void*)brk(end);
	if (!end)
	{
		return 0;
	}
#endif

	header	= (heap_header*)base;

	header->size	= heap_size;
	header->type	= HEAP_BLOCK_FREE;
	header->next	= NULL;
	header->prev	= NULL;

	list_head		= header;
	return 1;
}

