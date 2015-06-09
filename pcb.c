/*************************
 * * Daniel Collis
 * * 430133523
 * * COMP3520 Assignment 2
 * **************************/
/*******************************************************************

  OS Eercises - Project 2 - HOST dispatcher
 
  pcb - process control block functions for HOST dispatcher

   PcbPtr startPcb(PcbPtr process) - start (or restart) a process
    returns:
      PcbPtr of process
      NULL if start (restart) failed

   PcbPtr suspendPcb(PcbPtr process) - suspend a process
    returns:
      PcbPtr of process
      NULL if suspend failed

   PcbPtr terminatePcb(PcbPtr process) - terminate a process
    returns:
      PcbPtr of process
      NULL if terminate failed

   PcbPtr printPcb(PcbPtr process, FILE * iostream)
    - print process attributes on iostream
    returns:
      PcbPtr of process

   void printPcbHdr(FILE *) - print header for printPcb
    returns:
      void
      
   PcbPtr createnullPcb(void) - create inactive Pcb.
    returns:
      PcbPtr of newly initialised Pcb
      NULL if malloc failed

   PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process)
      - queue process (or join queues) at end of queue
      - enqueues at "tail" of queue. 
    returns head of queue

   PcbPtr deqPcb (PcbPtr * headofQ);
      - dequeue process - take Pcb from "head" of queue.
    returns:
      PcbPtr if dequeued,
      NULL if queue was empty
      & sets new head of Q pointer in adrs at 1st arg

 ********************************************************************

   version: 1.0 (exercise 7)
   date:    December 2003
   author:  Dr Ian G Graham, ian.graham@griffith.edu.au

 *******************************************************************/

#include "pcb.h"

/*******************************************************
 * PcbPtr startPcb(PcbPtr process) - start (or restart)
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if start (restart) failed
 ******************************************************/
PcbPtr startPcb (PcbPtr p) { 
    if(p) {
	    if (p->pid == 0) {                 // not yet started
	        switch (p->pid = fork ()) {    //  so start it
	            case -1: 
	                perror ("startPcb");
	                exit(1); 
	            case 0:                             // child 
	                p->pid = getpid();
	                p->status = PCB_RUNNING;
	                printPcbHdr(stdout);            // printout in child to
	                printPcb(p, stdout);            //  sync with o/p
	                fflush(stdout);
	                execvp (p->args[0], p->args); 
	                perror (p->args[0]);
	                exit (2);
	        }                                       // parent         
	
	    }
	    else { // already started & suspended so continue
	        kill (p->pid, SIGCONT);
	    }    
	    p->status = PCB_RUNNING;
    }

    return p; 
} 

/*******************************************************
 * PcbPtr suspendPcb(PcbPtr process) - suspend
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if suspend failed
 ******************************************************/
 PcbPtr suspendPcb(PcbPtr p) {
    if(p) {
        if(kill(p->pid, SIGTSTP))
		return NULL;	// Error

	int status;
	waitpid(p->pid, &status, WUNTRACED);
	p->status = PCB_SUSPENDED;
    }
    
    return p;
}
 
/*******************************************************
 * PcbPtr terminatePcb(PcbPtr process) - terminate
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if terminate failed
 ******************************************************/
PcbPtr terminatePcb(PcbPtr p) {
    if(p) {
        if(kill(p->pid, SIGINT))
		return NULL;	// Error

	int status;
	waitpid(p->pid, &status, WUNTRACED);
	p->status = PCB_TERMINATED;
    }

    return p;
}  

/*******************************************************
 * PcbPtr printPcb(PcbPtr process, FILE * iostream)
 *  - print process attributes on iostream
 *  returns:
 *    PcbPtr of process
 ******************************************************/
PcbPtr printPcb(PcbPtr p, FILE * iostream) {
	if(p) {
		fprintf(iostream, "%7d", p->pid);
		fprintf(iostream, "%7d", p->arrivaltime);
		fprintf(iostream, "%7d", p->priority);
		fprintf(iostream, "%7d", p->remainingcputime);
		if(p->memoryblock)
			fprintf(iostream, "%7d", p->memoryblock->offset);
		else
			fprintf(iostream, "%7s", "e");
		fprintf(iostream, "%7d", p->mbytes);
		fprintf(iostream, "%6d", p->req.printers);
		fprintf(iostream, "%7d", p->req.scanners);
		fprintf(iostream, "%7d", p->req.modems);
		fprintf(iostream, "%7d", p->req.cds);
		switch(p->status) {
			case PCB_UNINITIALIZED:
				fprintf(iostream, "%9s", "UNINITIALIZED");
				break;
			case PCB_INITIALIZED:
				fprintf(iostream, "%9s", "INITIALIZED");
				break;
			case PCB_READY:
				fprintf(iostream, "%9s", "READY");
				break;
			case PCB_RUNNING:
				fprintf(iostream, "%9s", "RUNNING");
				break;
			case PCB_SUSPENDED:
				fprintf(iostream, "%9s", "SUSPENDED");
				break;
			case PCB_TERMINATED:
				fprintf(iostream, "%9s", "TERMINATED");
				break;
			default:
				fprintf(iostream, "--------");
		}

		fprintf(iostream, "\n");
	}

	return p;
}
   
/*******************************************************
 * void printPcbHdr(FILE *) - print header for printPcb
 *  returns:
 *    void
 ******************************************************/  
void printPcbHdr(FILE * iostream) {  
	fprintf(iostream, "%7s", "pid");
	fprintf(iostream, "%7s", "arrive");
	fprintf(iostream, "%7s", "prior");
	fprintf(iostream, "%7s", "cpu");
	fprintf(iostream, "%7s", "offset");
	fprintf(iostream, "%7s", "MBytes");
	fprintf(iostream, "%7s", "prn");
	fprintf(iostream, "%7s", "scn");
	fprintf(iostream, "%8s", "modem");
	fprintf(iostream, "%5s", "cd");
	fprintf(iostream, "%8s", "status");

	fprintf(iostream, "\n");
}
       
/*******************************************************
 * PcbPtr createnullPcb() - create inactive Pcb.
 *
 * returns:
 *    PcbPtr of newly initialised Pcb
 *    NULL if malloc failed
 ******************************************************/
PcbPtr createnullPcb() {
    PcbPtr newprocessPtr;
      
    if ((newprocessPtr = (PcbPtr) malloc (sizeof(Pcb)))) {
        newprocessPtr->pid = 0;
        newprocessPtr->args[0] = DEFAULT_PROCESS;
        newprocessPtr->args[1] = NULL;
        newprocessPtr->arrivaltime = 0;
        newprocessPtr->priority = HIGH_PRIORITY;
        newprocessPtr->remainingcputime = 0;
        newprocessPtr->mbytes = 0;
        newprocessPtr->memoryblock = NULL;
        newprocessPtr->req.printers = 0;
        newprocessPtr->req.scanners = 0;
        newprocessPtr->req.modems = 0;
        newprocessPtr->req.cds = 0;
        newprocessPtr->status = PCB_UNINITIALIZED;
        newprocessPtr->next = NULL;
        return newprocessPtr;
    }
    perror("allocating memory for new process");
    return NULL;
}   

/*******************************************************
 * PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process)
 *    - queue process (or join queues) at end of queue
 * 
 * returns head of queue
 ******************************************************/
PcbPtr enqPcb(PcbPtr q, PcbPtr p) {
    PcbPtr h = q;
    
    p->next = NULL; 
    if (q) {
        while (q->next) q = q->next;
        q->next = p;
        return h;
    }
    return p;
}
/*******************************************************
 * PcbPtr deqPcb (PcbPtr * headofQ);
 *    - dequeue process - take Pcb from head of queue.
 *
 * returns:
 *    PcbPtr if dequeued,
 *    NULL if queue was empty
 *    & sets new head of Q pointer in adrs at 1st arg
 *******************************************************/
PcbPtr deqPcb(PcbPtr * hPtr) {
    PcbPtr p;
     
    if (hPtr && (p = * hPtr)) {
        * hPtr = p->next;
        return p;
    }
    return NULL;
}
