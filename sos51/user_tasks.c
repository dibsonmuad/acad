/*! user_tasks.c - Responsible for implementing all
  user level tasks in the system.
*/

#include <stdio.h>
#include "sos.h"

/*! Used by tasks 1 and 2 for mutual exclusion
 */
volatile Semaphore* gl_sem_count_lock;

/*!
  Counter accessed by tasks 1 and 2 to
  illustrate mutual exclusion. gl_counter
  is the critical resource. It could be a
  printer. Only one task can perform any operation
  on it.
 */
volatile unsigned char gl_counter;

extern void task_exit() __naked;
extern TCB* gl_task_queue[TASK_COUNT];

extern void putchar(char c);
extern char getchar(void);

extern void sem_wait(Semaphore* sem);
extern void sem_release(Semaphore* sem);

//! One wire interface to temperature sensor */
extern void Read_Temperature(void);

/*! Task 2(1) waits for the 'count lock' semaphore.
 * Once it receives the lock, it keeps de(in)crement-
 * -ing the gl_counter variable until it reaches 0(100)
 * During this time, task 1(2) will not be able to
 * modify the gl_counter (as it is programmed right
 * now).
 */
void task1_fn(void)
{
    // unsigned int counter = 0;
    sem_wait(gl_sem_count_lock);
    putchar('\n');
    putchar('\r');
    while (gl_counter < 100)
    {
        putchar('1');
        gl_counter++;
    }

    sem_release(gl_sem_count_lock);
    task_exit();
}

//! Please see task1_fn for documentation
void task2_fn(void)
{
    sem_wait(gl_sem_count_lock);
    putchar('\n');
    putchar('\r');
    while (gl_counter > 0)
    {
        putchar('2');
        gl_counter--;
    }
    sem_release(gl_sem_count_lock);
    task_exit();
}

/*! Calculate some statistics about various processes
 * 1. Context switches.
 * 2. CPU usage
 * 3. Stack space used
 * 4. Task State
 * This task should not be considered a user level task.
 * This is priviliged as interrupts are disabled. This is
 * because
 * If the task is switched out before it is finished cal-
 * -culating, the results will be erroneous.
 * For display purposes printf is used and it is not
 * reentrant. This has the potential to garble the output.
 */
void task3_fn(void)
{
    int counter = 0;
    TCB* p_task;
    EA  = 0;
    ET0 = 0;
    printf_small("\n\rID\tS_SIZE\tSTATE\tQ\tS\tY ");
    for (counter = 0; counter < TASK_COUNT; counter++)
    {
        p_task = gl_task_queue[counter];
        printf_small("\n\r%d\t%d\t%d\t%d\t%d\t%d",
                     p_task->id,
                     (p_task->pxstack - p_task->pxstack_start),
                     p_task->task_state,
                     p_task->n_quantums,
                     p_task->n_suspend,
                     p_task->n_yield);
    }
    Read_Temperature();
    task_exit(); // Re-enables timers
}

/*! This task never relinquishes its CPU time. It
 * always uses up its time quantum provided by the
 * CPU
 */
void task4_fn(void)
{
    unsigned int counter = 0;
    while (1)
    {
        putchar('4');
    }
}
