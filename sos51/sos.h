/*!
  This is the only header file used by all other files.
  User tasks are not dependent on this header file. However,
  some task management functions have to be referenced
  by user tasks.
*/
#ifndef SOS_H_
#define SOS_H_
#include <8051.h>

#define TASK_COUNT 5
#define STACK_SIZE 40

//! This is used to manipulate SFRs ex. SP 
#define DEREF(NAME) *((data unsigned char* )NAME)

/*! 
  Create a type t_task_fn. The startup task for any 
  function has  to match this signature
*/
typedef void (*t_task_fn)(void);

/*!
 * TCB - Task control block. Every task is created and 
 * and initialized within the context of the TCB.
 */
typedef struct 
 {
   /*! task id*/
   unsigned char id;
   /*! external stack pointer */
   unsigned char* pxstack;
   /*! start of stack. Used to keep track of stack size. Treat
    * stack contents as just bytes of data that need to be moved
    */
   const unsigned char* pxstack_start;
   /*! task state */
   enum TaskState task_state;
   /*! if suspended on a semaphore */
   unsigned char* sem_address;
   /*! the actual task that needs to be executed*/
   t_task_fn task_fn_ptr;
   /*! number of times the task used up its time slice */
   int n_quantums;
   /*! number of times it was suspended */
   int n_suspend; 
   /*! number of times it volantarily gave up cpu */
   int n_yield;
}TCB;

/*! The different states a task could be in 
 */
enum TaskState 
  { TS_RUNNING=0x01, 
    TS_READY=0x02, 
    TS_WAITING=0x04, 
    TS_SUSPEND=0x08, 
    TS_ISR=0x16 
  }; 


/*! Semaphore - An instance of this struct is created on the heap
 * whenever a task initializes a semaphore.
 */ 
typedef struct
{
  /*! initialize the semaphore with this value */
  unsigned char start_value;
  /*! reflects the number of tasks that can call the semaphore */
  unsigned char value;
  /*! A record of the tasks waiting on the semaphore */
  char* task_list;
}Semaphore;

/*! simple macro for assembly instructions */
#define ASM(instr) _asm			\
  instr					\
  _endasm;

#endif /*SOS_H_*/
