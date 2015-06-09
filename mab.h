/*************************
 * * Daniel Collis
 * * 430133523
 * * COMP3520 Assignment 2
 * **************************/
#ifndef MAB_H
#define MAB_H
/*******************************************************************

  OS Exercises - Project 2 - HOST dispatcher

  mab.h - Memory Allocation definitions and prototypes for HOST dispatcher

  MabPtr memChk (MabPtr arena, int size); - check for memory available
  int memChkMax (int size); - check for over max memory
  MabPtr memAlloc (MabPtr arena, int size); - allocate a memory block
  MabPtr memFree (MabPtr mab); - de-allocate a memory block
  MabPtr memMerge(Mabptr m); - merge m with m->next
  MabPtr memSplit(Mabptr m, int size); - split m into two
  void memPrint(MabPtr arena); - print contents of memory arena

  see mab.c for fuller description of function arguments and returns

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

#include <stdlib.h>
#include <stdio.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* memory management *******************************/

#define MEMORY_SIZE       1024
#define RT_MEMORY_SIZE    64
#define USER_MEMORY_SIZE  (MEMORY_SIZE - RT_MEMORY_SIZE)

struct mab {
    int offset;
    int size;
    int allocated;
    struct mab * next;
    struct mab * prev;
};

typedef struct mab Mab;
typedef Mab * MabPtr; 

/* memory management function prototypes ********/

MabPtr memInit(int);
MabPtr memChk(MabPtr, int);
int    memChkMax(int);  
MabPtr memAlloc(MabPtr, int);
MabPtr memFree(MabPtr);
MabPtr memMerge(MabPtr);   
MabPtr memSplit(MabPtr, int);
void   memPrint(MabPtr);
#endif
