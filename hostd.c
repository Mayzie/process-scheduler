/*************************
 * * Daniel Collis
 * * 430133523
 * * COMP3520 Assignment 2
 * **************************/
#define LOW_PRIORITY     (N_QUEUES - 1)
/*******************************************************************

OS Eercises - Project 2 - HOST dispatcher - Exercise 7

    hostd

        hostd is fcfs 'dispatcher' that reads in a list of 'jobs' from a file
        and 'dispatches' them in a first-come-first-served manner.

        time resolution is one second (although this can be changed). 

    usage
   
        hostd <dispatch file>
 
        where
            <dispatch file> is list of process parameters as specified 
                for assignment 2.

    functionality

    1. Initialize dispatcher queue;
    2. Fill dispatcher queue from dispatch list file;
    3. Start dispatcher timer;
    4. While there's anything in the queue or there is a currently running process:
        i. If a process is currently running;
            a. Decrement process remainingcputime;
            b. If times up:
                A. Send SIGINT to the process to terminate it;
                B. Free up process structure memory
       ii. If no process currently running &&
            dispatcher queue is not empty &&
            arrivaltime of process at head of queue is <= dispatcher timer:
            a. Dequeue process and start it (fork & exec)
            b. Set it as currently running process;
      iii. sleep for one second;
       iv. Increment dispatcher timer;
        v. Go back to 4.
    5. Exit

********************************************************************

version: 1.0 (exercise 7)
date:    December 2003
author:  Dr Ian G Graham, ian.graham@griffith.edu.au

*******************************************************************/

#include "hostd.h"
#include <stdbool.h>

#define VERSION "1.0"

/******************************************************
 
   internal functions
   
 ******************************************************/

char * StripPath(char*);
void PrintUsage(FILE *, char *);
void SysErrMsg(char *, char *);
void ErrMsg(char *, char *);

/******************************************************/

int queues_full(PcbPtr* feedback) {
	int i;
	for(i = 0; i < N_FB_QUEUES; ++i) {
		if(feedback[i]) 
			return i;
	}

	return -1;
}

int main (int argc, char *argv[]) {
    char * inputfile;             // job dispatch file
    FILE * inputliststream;

    PcbPtr inputqueue = NULL;     // input queue buffer
    PcbPtr userqueue = NULL;
    PcbPtr realtimequeue = NULL;
    PcbPtr feedbackqueues[N_FB_QUEUES];

    // initialise all feedbackqueue pointers to NULL
    int i;
    for(i = 0; i < N_FB_QUEUES; ++i) {
	    feedbackqueues[i] = NULL;
    }

    PcbPtr current_process = NULL; // current process
    PcbPtr process = NULL;        // working pcb pointer
    int timer = 0;                // dispatcher timer 

    MabPtr memory = memInit(MEMORY_SIZE);	// global memory pointer
    memory = memAlloc(memory, RT_MEMORY_SIZE);	// reserve a 64mb block for realtime processes
    RsrcPtr resources = rsrcInit(MAX_PRINTERS, MAX_SCANNERS, MAX_MODEMS, MAX_CDS);	// global resources pointer

//  0. Parse command line

    if (argc == 2) inputfile = argv[1];
    else PrintUsage (stderr, argv[0]);

//  1. Initialize dispatcher queue;
//     (already initialised in assignments above)
    
//  2. Fill dispatcher queue from dispatch list file;
    
    if (!(inputliststream = fopen(inputfile, "r"))) { // open it
        SysErrMsg("could not open dispatch list file:", inputfile);
        exit(2);
    }

    while (1) {
        int val_check;
        process = createnullPcb();
        if ((val_check = fscanf(inputliststream, "%d, %d, %d, %d, %d, %d, %d, %d",
          &(process->arrivaltime), &(process->priority),
          &(process->remainingcputime), &(process->mbytes),
          &(process->req.printers), &(process->req.scanners),
          &(process->req.modems), &(process->req.cds))) == 8) {
            process->status = PCB_INITIALIZED;
            inputqueue = enqPcb(inputqueue, process);
        }
        else if (val_check >= 0){
            free(process);
            ErrMsg("Invalid input file", inputfile);
            exit(2);
        }
        else if (ferror(inputliststream)) {
            free(process);
            SysErrMsg("Error reading input file", inputfile);
            exit(2);
        }
        else {
            free(process);
            break;
        }
    }

    	// 5. While there's anything in any of the queues, or there is a currently running process
	int fbqueue_fidx = 0;
	while(inputqueue || userqueue || realtimequeue || (current_process != NULL && current_process->status == PCB_RUNNING) || (fbqueue_fidx = queues_full(feedbackqueues)) >= 0) {
		while((inputqueue != NULL) && (inputqueue->arrivaltime <= timer)) {
			if(inputqueue->priority == RT_PRIORITY) {
				// 5.i.a. Real-time queue
				realtimequeue = enqPcb(realtimequeue, deqPcb(&inputqueue));
				realtimequeue->memoryblock = memory;	// ALL realtime processes will have 64mb, so give it the reserved block.
			} else	{
				// 5.i.b. User job queue
				userqueue = enqPcb(userqueue, deqPcb(&inputqueue));
			}
		}

		// 5.ii. Unload pending processes from the user job queue
		while(userqueue) {
			// 5.ii while head of user job queue can be allocated and resources are available
			if(memChk(memory, userqueue->mbytes) != NULL && rsrcChk(resources, userqueue->req)) {
				// 5.ii.a. Dequeue process from user job queue
				PcbPtr user_process = deqPcb(&userqueue);
				// 5.ii.b. Allocate memory to the process
				user_process->memoryblock = memAlloc(memory, user_process->mbytes);
				// 5.ii.c. Allocate I/O resoruces to the process
				rsrcAlloc(resources, user_process->req);
//				user_process->status = PCB_READY;
				// 5.ii.d. Enqueue on to appropriate priority feedback queue
				feedbackqueues[user_process->priority - 1] = enqPcb(feedbackqueues[user_process->priority - 1], user_process);	// -1, because indexing starts at 0
			} else
				break;	// Cannot allocate resources from head of queue
		}
		
		// 5.iii. If a process is currently running
		if(current_process != NULL && current_process->status == PCB_RUNNING) {
			// 5.iii.a. Decrement process remanining cpu time
			current_process->remainingcputime--;

			// 5.iii.b. If time's up
			if(current_process->remainingcputime <= 0) {
				// 5.iii.b.A. Send SIGINT to the process to terminate it
				terminatePcb(current_process);
				// 5.iii.b.B. Free memory and resources allocated to the process (user processes only)
				if(current_process->priority != RT_PRIORITY) {
					rsrcFree(resources, current_process->req);
					memFree(current_process->memoryblock);
					//current_process->memoryblock = NULL;
				}
				// 5.iii.b.C. Free up process structure memory
				free(current_process);
				current_process = NULL;
			} // 5.iii.c. Else if it is a user process and other processes are waiting in any of the queues
			else if(current_process->priority != RT_PRIORITY) {
				bool suspend = realtimequeue != NULL;
				// Check if there are any waiting process in any of the feedback queues
				if(realtimequeue == NULL) {
					int i;
					for(i = 0; i < N_FB_QUEUES; ++i) {
						if(feedbackqueues[i]) {
							suspend = (feedbackqueues[i]->status == PCB_READY || feedbackqueues[i]->status == PCB_SUSPENDED);
							if(suspend)
								break;
						}
					}
				}

				if(suspend) {
					// 5.iii.c.A. Send SIGTSTP to suspend it
					suspendPcb(current_process);
					// 5.iii.c.B. Reduce the priority of the process (if possible)...
					current_process->priority += (current_process->priority < (LOW_PRIORITY - 1)) ? 1 : 0;
					// 5.iii.c.B. ...and enqueue it back onto the appropriate feedback queue
					feedbackqueues[current_process->priority] = enqPcb(feedbackqueues[current_process->priority], current_process);
					current_process = NULL;
				}
			}
		}

		// 5.iv. If no process is currently running...
		if(current_process == NULL || current_process->status != PCB_RUNNING) {
			process = NULL;

			// 5.iv. ...and real time queue OR feedback queue are not empty
			if(realtimequeue) {
				// 5.iv.a. Dequeue a process from the highest priority queue that is not empty
				process = deqPcb(&realtimequeue);
			} else {
				// Find first non-empty feedback queue
				int i;
				for(i = 0; i < N_FB_QUEUES; ++i) {
					if(feedbackqueues[i]) {
						break;
					}
				}
				// 5.iv.a. Dequeue a process from the highest priority queue that is not empty and is ready
				if(i < N_FB_QUEUES) {
					PcbPtr tempPcb = deqPcb(&feedbackqueues[i]);
					process = tempPcb;
				}
			}

			if(process != NULL) {
				// 5.iv.b. If already started but suspended, restart it, else, start it
				startPcb(process);
				// 5.iv.c. Set it as the currently running process
				current_process = process;
			} else
				current_process = NULL;
		}

		// 5.v. Sleep for one second
		sleep(1);
		// 5.vi. Increment dispatcher timer
		timer += QUANTUM;
	}
    
	free(resources);
	free(memory);
    exit (0);
}    


/*******************************************************************

char * StripPath(char * pathname);

  strip path from file name

  pathname - file name, with or without leading path

  returns pointer to file name part of pathname
    if NULL or pathname is a directory ending in a '/'
        returns NULL
*******************************************************************/

char * StripPath(char * pathname) {
    char * filename = pathname;

    if (filename && *filename) {           // non-zero length string
        filename = strrchr(filename, '/'); // look for last '/'
        if (filename)                      // found it
            if (*(++filename))             //  AND file name exists
                return filename;
            else
                return NULL;
        else
            return pathname;               // no '/' but non-zero length string
    }                                      // original must be file name only
    return NULL;
}

/*******************************************************
 * print usage
 ******************************************************/
void PrintUsage(FILE * stream, char * progname) {
    if (!(progname = StripPath(progname))) {
        progname = DEFAULT_NAME;
    }
    
    fprintf(stream,"\n"
      "%s process dispatcher (version " VERSION "); usage:\n\n"
      "  %s <dispatch file>\n"
      " \n"
      "  where <dispatch file> is list of process parameters \n\n",
      progname,progname);

    exit(127);
}
/********************************************************
 * print an error message on stderr
 *******************************************************/

void ErrMsg(char * msg1, char * msg2) {
    if (msg2) {
        fprintf(stderr,"ERROR - %s %s\n", msg1, msg2);
    }
    else {
        fprintf(stderr,"ERROR - %s\n", msg1);
    }
    return;
}

/*********************************************************
 * print an error message on stderr followed by system message
 *********************************************************/

void SysErrMsg(char * msg1, char * msg2) {
    if (msg2) {
        fprintf(stderr,"ERROR - %s %s; ", msg1, msg2);
    }
    else {
        fprintf(stderr,"ERROR - %s; ", msg1);
    }
    perror(NULL);
    return;
}
