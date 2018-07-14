/*
threads.c - source code to implement threads object which provides the methods 
  to implement a thread scheduler in user space

AUTHOR: Jonathan Chamberlain (jdchambo@bu.edu)
Developed for EC 440
*/

// helper function written by instructors to encrypt pointers
int ptr_mangle(int p)
{
	unsigned int ret;
	asm(" movl %1, %%eax;\n"
	" xorl %%gs:0x18, %%eax;\n"
	" roll $0x9, %%eax;\n"
	" movl %%eax, %0;\n"
	: "=r"(ret)
	: "r"(p)
	: "%eax"
	);
	return ret;
}

#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// define struct for thread control blocks

struct Thread {
  pthread_t tid;
  jmp_buf tenv; 
  void *tstack;
  int status;
};


// define struct for scheduluer handler
struct sigaction scheduler;

// define global constants
#define MAXTHREADS 129 // 128 threads + thread0 for main
#define READY 1
#define RUNNING 2
#define EXIT 0
#define FREQ 50000 // 50 ms switches - ualarm defined in microseconds

// declare global variables
pthread_t gCurrent = 0;
struct Thread threads[MAXTHREADS]; 
int numthreads = 0; // number of threads created by main

// scheduler to do RR switching
void schedule () {
  // triggered on SIGALRM
  if (!setjmp(threads[gCurrent].tenv)) {
    // called by current thread saving/exiting
    // save current thread state
    if (threads[gCurrent].status == RUNNING) {
      threads[gCurrent].status = READY;
    }
    // determine next thread
    do {
      gCurrent++;
      if (gCurrent == numthreads) gCurrent = 0;
    } while(threads[gCurrent].status != READY);
    threads[gCurrent].status = RUNNING;
    longjmp(threads[gCurrent].tenv, 1);
  }
}

// wrapper to call schedule when alarm goes off - schedule takes no arguments
void schhandler(int signum) {
  schedule();
}

// get ID of currently running thread
pthread_t pthread_self(void) {
  return gCurrent;
}

// create new thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void 
  *(*start_routine) (void *), void *arg) {
   // if first call, init scheduler, thread status, make TCB for main program
   int i;
   if (numthreads == 0) {
     numthreads++;
     i = 1;
     while(i < MAXTHREADS) {
       threads[i].status = EXIT;
       i++;
     }
     // TCB for main program
     i = 0;
     threads[i].tid = i;
     threads[i].status = READY;
     setjmp(threads[i].tenv);  
     // Initialize scheduler
     scheduler.sa_handler = *schhandler;
     scheduler.sa_flags = SA_NODEFER;
     sigaction(SIGALRM,&scheduler,NULL);
     ualarm(FREQ, FREQ);
   }
   // create new TCB for new thread, if threads available
   if (numthreads < MAXTHREADS) {
      i = numthreads;
      numthreads++;
      threads[i].tid = i;
      // set and manipulate stack in TCB
      threads[i].tstack = (void*) malloc(32767);
      unsigned long int *p = threads[i].tstack + 32767;
      p -= 1;
      *p = (unsigned long int) arg;
      p -= 1;
      *p = (unsigned long int) pthread_exit;
      // set registers in TCB
      setjmp(threads[i].tenv);
      threads[i].tenv[0].__jmpbuf[4] = ptr_mangle((unsigned long int) p);
      threads[i].tenv[0].__jmpbuf[5] = ptr_mangle((unsigned long int) start_routine);
      threads[i].status = READY;
   }
   *thread = i;
   return 0;
}

// exit current thread - main will exit on hitting return in its process
void pthread_exit(void *value_ptr) {
  // free resources for current thread
  free(threads[gCurrent].tstack);
  // set thread's state to EXIT
  threads[gCurrent].status = EXIT;
  // call scheduler
  schedule();
  __builtin_unreachable();
}


