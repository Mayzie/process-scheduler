/*************************
* Daniel Collis
* 430133523
* COMP3520 Assignment 2
**************************/

/*******************************************************************

  OS Exercises - Project 2 - HOST dispatcher

    rsrc - resource management routines for the HOST dispatcher

    int rsrcChk (RsrcPtr available, Rsrc claim)
       - check that resources are available now 
      returns:
        TRUE or FALSE - no allocation is actally done

    int rsrcChkMax (Rsrc claim)
       - check that resources will ever be available
      returns:
        TRUE or FALSE

    int rsrcAlloc (RsrcPtr available, Rsrc claim)
       - allocate resources
      returns:
        TRUE or FALSE if not enough resources available

    void rsrcFree (RsrcPtr available, Rsrc claim);
       - free resources
      returns NONE

********************************************************************

  version: 1.0 bis (exercise 11 code template)
  date:    December 2003
  author:  Dr Ian G Graham, ian.graham@griffith.edu.au
  history:
     v1.0:     Original for exercises 11 and full project dispatcher
     v1.0 bis: Code template for exercise 11

*******************************************************************/

#include "rsrc.h"
                                          
RsrcPtr rsrcInit(int printers, int scanners, int modems, int cds) {
	RsrcPtr result = (RsrcPtr) malloc(sizeof(Rsrc));

	result->printers = printers;
	result->scanners = scanners;
	result->modems = modems;
	result->cds = cds;

	return result;
}

/*******************************************************
 * int rsrcChk (RsrcPtr available, Rsrc claim)
 *    - check that resources are available
 *
 * returns:
 *    TRUE or FALSE - no allocation is actally done
 *******************************************************/
int rsrcChk(RsrcPtr available, Rsrc claim)
{
	bool result = false;
	result = available->printers >= claim.printers;
	result = result && available->scanners >= claim.scanners;
	result = result && available->modems >= claim.modems;
	result = result && available->cds >= claim.cds;

	return result;
}
    
/*******************************************************
 * int rsrcChkMax (Rsrc claim)
 *    - check that resources could be available
 *
 * returns:
 *    TRUE or FALSE - no allocation is actally done
 *******************************************************/
int rsrcChkMax(Rsrc claim)
{
	return 0;
}
    
/*******************************************************
 * int rsrcAlloc (RsrcPtr available, Rsrc claim)
 *    - allocate resources
 *
 * returns:
 *    TRUE or FALSE if not enough resources available
 *******************************************************/
int rsrcAlloc(RsrcPtr available, Rsrc claim)
{
	bool result = rsrcChk(available, claim);
	if(result) {
		available->printers -= claim.printers;
		available->scanners -= claim.scanners;
		available->modems -= claim.modems;
		available->cds -= claim.cds;
	}

	return result;
}

/*******************************************************
 * void rsrcFree (RsrcPtr available, Rsrc claim);
 *    - free resources
 *
 * returns NONE
 *******************************************************/
void rsrcFree(RsrcPtr available, Rsrc claim)
{
	available->printers += claim.printers;
	available->scanners += claim.scanners;
	available->modems += claim.modems;
	available->cds += claim.cds;

	if(available->printers > MAX_PRINTERS)
		available->printers = MAX_PRINTERS;
	if(available->scanners > MAX_SCANNERS)
		available->scanners = MAX_SCANNERS;
	if(available->modems > MAX_MODEMS)
		available->modems = MAX_MODEMS;
	if(available->cds > MAX_CDS)
		available->cds = MAX_CDS;
}

