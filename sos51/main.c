#include <stdio.h>
#include "sos.h"

/*! Keeps track of currently running task. Marked volatile
 * as it is used from timer isr.
 */
volatile TCB* gl_current_task;

/*! This variable is set to whereever the
 * stack pointer points to after initialization
 */
volatile unsigned char gl_start_of_stack;

//! Contains a list of all tasks created
volatile TCB* gl_task_queue[TASK_COUNT];

/*! An array of function pointers used to hold
 * pointers to the actual task functions
 */
void (*task_fn_array[TASK_COUNT])(void);
//! External reference to task_create fn in task.c
extern void task_create(int id, t_task_fn task_fn);

// Semaphore fn.s
extern Semaphore* sem_create(unsigned char);
extern Semaphore* gl_sem_count_lock;

// Interrupts
void timer0_isr(void) interrupt TF0_VECTOR _naked;
void serial_isr(void) using(2) interrupt SI0_VECTOR;

// User Tasks
extern void task1_fn(void);
extern void task2_fn(void);
extern void task3_fn(void);
extern void task4_fn(void);

void putchar(char c)
{
    while (!TI)
        ;     // wait for transmitter to be ready
    TI   = 0; // reset transmit flag
    SBUF = c; // write character to transmit buffer
}

char getchar(void)
{
    while (!RI)
        ;        // wait for character to be received
    RI = 0;      // reset receive flag
    return SBUF; // retrieve character and return
}

/*! System idle task. This task will be executed
 * if no other task is running
 */
void task0_fn(void)
{
    printf_small("\n\r Task 0 Idling...");
    while (1)
        ;
}

/*!
 * Contains the main function and other startup code
 */
void main(void)
{
    int task_id;
    /* Define ar0..7 for stack operations. The asm generates
       these variables without defining them anywhere in its
       code. The 8051 FAQ has the following to say on this
       "The special assembler symbols AR0 ... AR7 are simply
       the absolute DATA addresses of registers R0 ... R7.
       Although there is no instruction PUSH R5 with true register
       addressing, you can do it with PUSH AR5 (= direct addressing)!"
       So I'm creating definitions for them so the assembler recognizes
       the variables IT creates!!
    */
    _asm ar0 = 0x00 ar1 = 0x01 ar2 = 0x02 ar3 = 0x03 ar4 = 0x04 ar5 = 0x05 ar6 =
        0x06 ar7 = 0x07 _endasm;

    /* Add task function pointers  */
    task_fn_array[0] = task0_fn;
    task_fn_array[1] = task1_fn;
    task_fn_array[2] = task2_fn;
    task_fn_array[3] = task3_fn;
    task_fn_array[4] = task4_fn;

    /* Create Tasks (Allocate, initialize) */
    for (task_id = 0; task_id <= 4; task_id++)
        task_create(task_id, task_fn_array[task_id]);

    /* Initialize Semaphore. Initialization can be done by the
       user task too. But I'm treating a semaphore as a system
       resource so tasks in this system will have only a
       limited number of semaphores to choose from and they have
       to know the semaphore names in advance.
     */
    gl_sem_count_lock = sem_create(1);

    /* Start the idle task */
    gl_start_of_stack = SP; // For all tasks, the stack starts here.

    /* Enable Timer0 */
    TMOD &= ~0x0F;
    TMOD |= T0_M0;
    /* Initial values for 20ms */
    // TL0=0xC3; TH0=0xB7;
    TL0 = 0xB3;
    TH0 = 0xB7;
    /* Assign a priority and enable the timer interrupt */
    PT0 = 1;
    ET0 = 1;

    /* Start the timer */
    TR0 = 1;

    /* Enable interrupts globally */
    EA = 1;

    /* Start the idle task */
    gl_current_task = gl_task_queue[0];
    (*gl_current_task->task_fn_ptr)();
}
