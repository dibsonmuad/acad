//! task.c - Implements "process management"
#include <malloc.h>
#include <stdio.h>
#include "sos.h"
/*! This is a small 'helper' macro for push-ing registers onto the stack.
 */
#define PUSH(reg) _asm push reg _endasm;

#define POP(reg) _asm pop reg _endasm;

/*! Pushes all relevant registers onto the stack.
 * This has to be a macro. I tried making this a
 * naked function. But had trouble maintaining
 * stack integrity.
 */
#define SAVE_REGISTERS                                                         \
    PUSH(acc)                                                                  \
    PUSH(b)                                                                    \
    PUSH(dpl)                                                                  \
    PUSH(dph)                                                                  \
    PUSH(ar2)                                                                  \
    PUSH(ar3)                                                                  \
    PUSH(ar4)                                                                  \
    PUSH(ar5)                                                                  \
    PUSH(ar6)                                                                  \
    PUSH(ar7)                                                                  \
    PUSH(ar0)                                                                  \
    PUSH(ar1)                                                                  \
    PUSH(psw)                                                                  \
    PSW = 0;                                                                   \
    PUSH(_bp)

/*! Pops the pushed registers from the stack. These
 * are restored from external memory first
 */
#define RESTORE_REGISTERS                                                      \
    POP(_bp)                                                                   \
    POP(psw)                                                                   \
    POP(ar1)                                                                   \
    POP(ar0)                                                                   \
    POP(ar7)                                                                   \
    POP(ar6)                                                                   \
    POP(ar5)                                                                   \
    POP(ar4)                                                                   \
    POP(ar3)                                                                   \
    POP(ar2)                                                                   \
    POP(dph)                                                                   \
    POP(dpl)                                                                   \
    POP(b)                                                                     \
    POP(acc)

/*! Copy contents from stack to external memory
 * Helper macro for STORE_STACK. Broken up for
 * possible reuse.
 */
#define STACK_COPY_ITOX                                                        \
    {                                                                          \
        gl_current_task->pxstack++;                                            \
        *gl_current_task->pxstack = DEREF(SP);                                 \
        SP--;                                                                  \
    }

/*! Copy contents to stack from external memory
 * Helper macro for RESTORE_STACK. Broken up for
 * possible reuse.
 */
#define STACK_COPY_XTOI                                                        \
    {                                                                          \
        SP++;                                                                  \
        DEREF(SP) = *(gl_current_task->pxstack);                               \
        /*putchar('\n');*/                                                     \
        (gl_current_task->pxstack)--;                                          \
    }

/*!
 * Writes stack to external memory
 * stack grows upward. SP always points to the next empty location.
 * Eg. If stack starts at 0x007, and there is one element on the
 * stack, SP points to 0x008.
 */
#define SAVE_STACK                                                             \
    {                                                                          \
        gl_current_task->pxstack = gl_current_task->pxstack_start;             \
        do                                                                     \
            STACK_COPY_ITOX                                                    \
        while (SP > gl_start_of_stack);                                        \
        printf_small("\n\rStored %d elements for task %d",                     \
                     gl_current_task->pxstack                                  \
                         - gl_current_task->pxstack_start,                     \
                     gl_next_task_id);                                         \
    }

/*! Restore the contents of stack from external RAM */
#define RESTORE_STACK                                                          \
    {                                                                          \
        SP = gl_start_of_stack;                                                \
        do                                                                     \
            STACK_COPY_XTOI                                                    \
        while (gl_current_task->pxstack > gl_current_task->pxstack_start);     \
        printf_small("\n\rRestored %d elements for task %d",                   \
                     SP - gl_start_of_stack,                                   \
                     gl_next_task_id);                                         \
    }

/*! Used for debugging purposes. The stack counter keeps a running
 * count of the total number of elements popped or pushed to
 * the stack
 */
volatile unsigned char gl_stack_counter = 0;

/*!
 * The scheduling algorithm decides which task to enable
 * next. This variable is set on a suspend, preempt or exit task.
 */
volatile unsigned char gl_next_task_id = 0;

extern TCB* gl_current_task;
extern unsigned char gl_start_of_stack;
extern TCB* gl_task_queue[TASK_COUNT];

/*! Allocates space for a task in external memory.
 * Puts the task in the READY state immediately
 * after creating the task. Initializes all other
 * members of the TCB
 */
void task_create(unsigned char id, t_task_fn task_fn_ptr)
{
    unsigned int task_fn_addr;
    unsigned char reg_counter = 0;
    TCB* new_task             = (TCB*)malloc(sizeof(TCB));
    new_task->id              = id;

    // Allocate space for its stack
    new_task->pxstack = (xdata unsigned char*)malloc(STACK_SIZE);

    // Add task to the task queue
    gl_task_queue[id] = new_task;

    // We shouldn't lose track of where the stack begins.
    // This is useful while saving and restoring the stack
    new_task->pxstack_start = new_task->pxstack;

    // We assume that the task is ready to run immediately
    // after it is created.
    new_task->task_state  = TS_READY;
    new_task->task_fn_ptr = task_fn_ptr;
    new_task->n_suspend   = 0;
    new_task->n_quantums  = 0;
    new_task->n_yield     = 0;

    // Initialize a dummy stack so they can be "pop"ped off after
    // a switch. Realized this needs to be done after
    // quite a bit of debugging
    while (reg_counter++ <= 13)
    {
        new_task->pxstack++;
        *new_task->pxstack = 0x00;
    }
    task_fn_addr = (unsigned int)*new_task->task_fn_ptr;

    // High 8 bytes go lower in the stack.
    new_task->pxstack++;
    *new_task->pxstack = (unsigned char)(task_fn_addr >> 8);

    // Low 8 bytes.
    new_task->pxstack++;
    *new_task->pxstack = (unsigned char)(task_fn_addr);
}

/*!
 * Find the next task in the READY state. If there are
 * no user tasks available, return the kernel task. This
 * is definitely one part of the kernel that needs to be
 * implemented as a proper state machine.
 */
unsigned char next_ready_task()
{
    volatile unsigned char proc_id = (gl_current_task->id % TASK_COUNT) + 1;
    unsigned char count            = 0;
    while (1)
    {
        if (count > TASK_COUNT)
            return 0; // The idle task
        // you'll hopefully hit one of the return statements !!
        else if (proc_id > TASK_COUNT - 1)
            proc_id = 0;
        // If the next task is ready, send it
        else if (proc_id == 0) // skip the idle task
        {
            proc_id++;
            count++;
        }
        else
        {
            if (gl_task_queue[proc_id]->task_state == (unsigned char)TS_READY)
                return proc_id;
            else
            {
                proc_id++;
                count++;
                continue;
            }
        }
    }
}

/*! A task can be suspended pending a semaphore wait
 * How do you suspend a task? Just mark it suspend.
 * Find the next task that is READY and run that task
 * Whenever a semaphore becomes free, call a fn. that
 * checks to see if there is any task waiting on that
 * semaphore and wake that task.
 * Make sure you push the reqd. variables on the stack
 */
void suspend_task(void) _naked
{
    EA               = 0; /*disable interrupts*/
    ET0              = 0;
    gl_stack_counter = 0;
    printf_small("\n\rIn Suspend. Active Task: %d\0", gl_current_task->id);
    SAVE_REGISTERS;
    SAVE_STACK;
    /*Set this process state to SUSPEND*/
    gl_current_task->task_state = TS_SUSPEND;
    gl_current_task->n_suspend++;
    /* look for next READY task
       Some regs are going to get messed up here,
       but we don't care since we've save them */
    gl_next_task_id = next_ready_task();
    // putchar(gl_next_task_id + 0x30);
    gl_current_task = gl_task_queue[gl_next_task_id];
    RESTORE_STACK;
    RESTORE_REGISTERS;
    printf_small("\n\rOut of Suspend. Active Task: %d", gl_next_task_id, SP);
    getchar();
    TL0 = 0xC3;
    TH0 = 0xB7; // reload value
    TF0 = 0;    // Reset the timer
    ET0 = 1;
    EA  = 1;
    ASM(ret)
}

/*! A processor might voluntarily give up cpu if it is
 * done with its processing. In this kernel, however,
 * launched tasks never really die. A task that exits is
 * resurrected on the stack and its status is READY.
 * In the next round robin allocation, the task will
 * run as if it is being invoked for the first time.
 */
void task_exit() _naked
{
    // this is very close to task create, but there is
    // also a context switch involved where the next
    // ready task is chosen to run.
    unsigned int task_fn_addr;
    unsigned char reg_counter  = 0;
    unsigned char next_char_id = 0;
    EA                         = 0;
    ET0                        = 0;
    printf_small("\n\rTask %d exiting", gl_current_task->id);
    // Reset xstack ponter
    gl_current_task->pxstack = gl_current_task->pxstack_start;

    // Keep READY status.
    gl_current_task->task_state = TS_READY;
    gl_current_task->n_yield++;
    // Initialize the dummy stack just as in task_create
    while (reg_counter++ <= 13)
    {
        gl_current_task->pxstack++;
        *gl_current_task->pxstack = 0x00;
    }
    task_fn_addr = (unsigned int)*gl_current_task->task_fn_ptr;

    // High 8 bytes go later in the actual stack.
    gl_current_task->pxstack++;
    *gl_current_task->pxstack = (unsigned char)(task_fn_addr >> 8);

    // Low 8 bytes.
    gl_current_task->pxstack++;
    *gl_current_task->pxstack = (unsigned char)(task_fn_addr);
    // you don't really care about the stack for the
    // current task so we don't save_regs or save_stack
    // Who's next?
    gl_next_task_id = next_ready_task();
    // putchar(gl_next_task_id + 0x30);
    gl_current_task = gl_task_queue[gl_next_task_id];
    RESTORE_STACK;
    RESTORE_REGISTERS;
    printf_small("\n\rOut of task_exit. Starting task %d", gl_next_task_id);
    getchar();
    TL0 = 0xC3;
    TH0 = 0xB7; // reload value
    TF0 = 0;    // Reset the timer
    ET0 = 1;
    EA  = 1;
    ASM(ret)
}

/*! Called from Timer interrupt. Hence this is implemented
 * as a macro.
 */
#define PREEMPT_TASK                                                           \
    {                                                                          \
        SAVE_REGISTERS;                                                        \
        SAVE_STACK;                                                            \
        /*Set this process state to READY*/                                    \
        gl_current_task->task_state = TS_READY;                                \
        gl_current_task->n_quantums++;                                         \
        /* Look for next READY task                                            \
           Some regs are going to get messed up here,                          \
           but we don't care since we've save them*/                           \
        gl_next_task_id = next_ready_task();                                   \
        /*putchar(gl_next_task_id + 0x30);	*/                                  \
        gl_current_task = gl_task_queue[gl_next_task_id];                      \
        RESTORE_STACK;                                                         \
        RESTORE_REGISTERS;                                                     \
    }

/* This function is kept for debugging purposes
 */
// void preempt_task() _naked
//{
//  SAVE_REGISTERS;
//  SAVE_STACK;
//  gl_current_task->task_state = TS_READY;
//  gl_current_task->n_quantums++;
//  gl_next_task_id = next_ready_task();
//  gl_current_task = gl_task_queue[gl_next_task_id];
//  RESTORE_STACK;
//  RESTORE_REGISTERS;
//  ASM(ret);
//}

/*! A Timer interrupt occurs every 20 ms. This interrupt
 * switches the current task with the next task indicated
 * by the scheduler.
 */
void timer0_isr(void) interrupt TF0_VECTOR _naked
{
    EA  = 0; /*disable interrupts*/
    ET0 = 0;
    printf_small("\n\rTimer Interrupting task %d", gl_current_task->id);
    gl_stack_counter = 0;
    PREEMPT_TASK;
    printf_small("\n\rTimer Starting Task %d", gl_next_task_id);
    getchar();
    TF0 = 0;
    TL0 = 0xC3;
    TH0 = 0xB7; // reload value
    ET0 = 1;
    EA  = 1;
    ASM(reti)
}
