/*************************
 * * Daniel Collis
 * * 430133523
 * * COMP3520 Assignment 2
 * **************************/

/*******************************************************************

  OS Exercises - Project 2 - HOST dispatcher

   mab - memory management functions for HOST dispatcher

   MabPtr memChk (MabPtr arena, int size);
      - check for memory available
 
    returns address of "First Fit" block or NULL

   int memChkMax (int size);
      - check for over max memory
 
    returns TRUE/FALSE OK/OVERSIZE

   MabPtr memAlloc (MabPtr arena, int size);
      - allocate a memory block
 
    returns address of block or NULL if failure

   MabPtr memFree (MabPtr mab);
      - de-allocate a memory block
 
    returns address of block or merged block

   MabPtr memMerge(Mabptr m);
      - merge m with m->next
 
    returns m

   MabPtr memSplit(Mabptr m, int size);
      - split m into two with first mab having size
  
    returns m or NULL if unable to supply size bytes

   void memPrint(MabPtr arena);
      - print contents of memory arena
   no return

 ********************************************************************
 
 version: 1.0 bis (exercises 10 & 11 code template)
 date:    December 2003
 author:  Dr Ian G Graham, ian.graham@griffith.edu.au
 history:
     v1.0:     Original for exercise 10
     v1.0 bis: Code template for exercises 10 & 11
 
 Note: This code template is based on the original version. It is
 edited and distributed by the COMP3520 teaching staff for use
 in COMP3520 labs.
 
 *******************************************************************/

#include "mab.h"

// static MabPtr next_mab = NULL;    // Is this value always reasonable?

MabPtr memInit(int size) {
	MabPtr memory = (MabPtr) malloc(sizeof(Mab));
	memory->offset = 0;
	memory->size = size;
	memory->allocated = FALSE;
	memory->next = NULL;
	memory->prev = NULL;

	return memory;
}

/*******************************************************
 * MabPtr memChk (MabPtr arena, int size);
 *    - check for memory available
 *
 * returns address of "First Fit" block or NULL
 *******************************************************/
MabPtr memChk(MabPtr arena, int size)
{
	while(arena) {
		if((arena->size >= size) && (arena->allocated == FALSE))
			return arena;

		arena = arena->next;
	}

	return NULL;
}
      
/*******************************************************
 * int memChkMax (int size);
 *    - check for memory available
 *
 * returns TRUE/FALSE
 *******************************************************/
int memChkMax(int size)
{
	return 0;
}      

/*******************************************************
 * MabPtr memAlloc (MabPtr arena, int size);
 *    - allocate a memory block
 *
 * returns address of block or NULL if failure
 *******************************************************/
MabPtr memAlloc(MabPtr arena, int size)
{
	if(size > 0) {
		MabPtr temp = memChk(arena, size);
		if(temp) {
			temp = memSplit(temp, size);
			temp->allocated = TRUE;
			return temp;
		}
	}

	return NULL;
}

/*******************************************************
 * MabPtr memFree (MabPtr mab);
 *    - de-allocate a memory block
 *
 * returns address of block or merged block
 *******************************************************/
MabPtr memFree(MabPtr m)
{
	if(m != NULL) {
		m->allocated = FALSE;
		MabPtr prev = m->prev;
		m = memMerge(m);
		prev = memMerge(prev);
	}

	return NULL;
}
      
/*******************************************************
 * MabPtr memMerge(Mabptr m);
 *    - merge m with m->next
 *
 * returns m
 *******************************************************/
MabPtr memMerge(MabPtr m)
{
	if(m) {
	       if(m->next && m->allocated == FALSE && m->next->allocated == FALSE) {
			MabPtr temp = m->next;
			m->size += temp->size;
			m->next = temp->next;
			if(temp->next)
				temp->next->prev = m;
			free(temp);
			return m;
	       }
	}

	return NULL;
}

/*******************************************************
 * MabPtr memSplit(MabPtr m, int size);
 *    - split m into two with first mab having size
 *
 * returns m or NULL if unable to supply size bytes
 *******************************************************/
MabPtr memSplit(MabPtr m, int size)
{
	if(m) {
		if(m->size == size) {
			return m;
		}

		MabPtr new_block = memInit(m->size - size);
		m->size -= size;
		new_block->next = m->next;
		new_block->offset = m->offset + size;
		m->next = new_block;
		new_block->prev = m;
		if(new_block->next != NULL)
			new_block->next->prev = new_block;
	
		return m;
	} else
		return NULL;
}

/*******************************************************
 * void memPrint(MabPtr arena);
 *    - print contents of memory arena
 * no return
 *******************************************************/
void memPrint(MabPtr arena) // Do you really need this?
{
    while(arena) {
        printf("offset%7d: size%7d - ",arena->offset, arena->size);
        if (arena->allocated) printf("allocated\n");
        else printf("free\n");
        arena = arena->next;
    }
}

