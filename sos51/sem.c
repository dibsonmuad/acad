//! sem.c - implements semaphore support
#include "sos.h"
#include <malloc.h>
#include <stdio.h>

extern volatile TCB* gl_current_task;
extern volatile TCB* gl_task_queue[TASK_COUNT];
extern void suspend_task() _naked;

/*! Returns a semaphore. Initialize it with the total
 * number of concurrent processes that can access 
 * the resource
 */
Semaphore* sem_create(unsigned char value)
{
  //internal RAM location.
  unsigned char counter;
  Semaphore* sem = (Semaphore*) malloc(sizeof(Semaphore));
  sem->start_value = value;
  sem->value = value;
  //Contains the list of tasks that are waiting on the semaphore
  //this is a poor man's list
  sem->task_list = (char*) malloc(sizeof(char) * value);
  //Initially no task is waiting on the semaphore (-1)
  for(counter=0; counter < sem->value; counter++)
    sem->task_list[counter] = -1;
  //printf_tiny("\n\rSem created at %x init val:%d", (unsigned int)sem, *sem);
  return sem;
}

/*! Any task that needs access to a protected resource 
 * has to gain access after receiving the semaphore
 * The value with which the semaphore was initialized
 * is decremented by one. If the semaphore value was 
 * zero, the task will be 'suspended'. Obtaining a 
 * semaphore is an atomic process. Hence interrupts
 * are disabled.
 */
void sem_wait(Semaphore* sem)
{
  unsigned char counter=0;
  EA = 0;
  //printf_tiny("\n\rInitial %x, %d\n", (unsigned int)sem, *sem);
  if(sem->value <= 0)
    {
      printf_small("\n\rNo semaphore for task %d!\0", gl_current_task->id);
      
      //append the task id to the end of the semaphore's
      //task id list
      for (counter = 0; counter< sem->start_value; counter++)
	if(sem->task_list[counter] = -1)
	  sem->task_list[counter] = gl_current_task->id;
      
      //Suspend the task
      suspend_task();
      //This statement will be executed once the task comes out of
      //suspended state. The suspended task will still have to fight for 
      //the semaphore!
      sem_wait(sem);
      return; //Explicit return necessary!
    }
  else
    {
      sem->value -= 1;
      printf_small("\n\rTask %d Got semaphore\n", gl_current_task->id);
      EA = 1; 
      return; 
    }
}

/*! When a task releases a semaphore, the semaphore's
 * value is incremented by 1. The first task in the 
 * list of tasks waiting on the semaphore is moved 
 * to ready state. Releasing a semaphore has to be 
 * atomic. Hence interrupts are disabled here.
 */
void sem_release(Semaphore* sem)
{
  unsigned char counter=0;
  EA = 0;
  sem->value += 1;
  //Now check to see if any tasks are waiting on this semaphore,
  //if so, change the first task's status to ready.
  printf_small("\n\rIn sem release: ");
  for(counter=0; counter<sem->start_value; counter++)
    {
      printf_small("| %d |", sem->task_list[counter]);
      if(sem->task_list[counter] != -1)
	{
	  /* The rr scheduler will pick up this task the next time around */
	  gl_task_queue[sem->task_list[counter]]->task_state = TS_READY;
	  sem->task_list[counter] = -1;
	  break;
      }
    }
  printf_small("\n\rTask %d released sem_lock, sem value: %d", gl_current_task->id, sem->value);     
  EA = 1;
}

