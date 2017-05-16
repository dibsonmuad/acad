#ifndef __quads_hh_
#define __quads_hh_

#define Q_JMP 0
#define Q_JEQ 1

#define Q_READ 2
#define Q_WRITE 3
#define Q_READLN 4
#define Q_WRITELN 5
#define Q_PLUS 6
#define Q_MINUS 7
#define Q_MUL 8
#define Q_DIV 9
#define Q_ASSGN 10

#define Q_EQ 11
#define Q_GTE 12
#define Q_LTE 13
#define Q_NEQ 14
#define Q_GT 15
#define Q_LT 16

/*Calculates the address of an ID */
#define Q_ADDR 17

/*Calculates the value at some address */
#define Q_VADDR 18

/*when op1 and/or op2 are empty, I pass this value*/
#define DUMMY 9999

#define SEPERATOR 100002

#define TRUE 10000
#define FALSE 10001
/*This is for the jump statements*/
#define UNKNOWN 667
#endif
