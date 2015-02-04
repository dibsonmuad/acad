#ifndef __emu__hh__
#define __emu__hh__
#include <stdio.h>
#include <stdlib.h>
#define OP_LD         0
#define OP_LDA        1 
/* load an immediate value into the compiler It is also used to
   load the base address of a variable/memory location
*/
#define OP_LDI        2
 
//#define OP_LDB 

#define OP_ST         3
#define OP_STA        4

#define OP_ADD        5     
#define OP_SUB        6 
#define OP_MUL        7   
#define OP_DIV        8 

#define OP_BLT        9 
#define OP_BGT       10
#define OP_BEQ       11  
#define OP_B         12

#define OP_READ      13
#define OP_WRITE     14
#define OP_NEWLINE   15

typedef struct {
  int opcode;
  int op1reg;
  int op2reg;
  int op1mem;
  int op2mem;
  int sign;
} decoded_instruction;

/* load the program into memory (the array p)*/
int load(char* c);

/* fetch the next instruction to be executed */
int fetch();

/* decodes the instruction register */
decoded_instruction decode(int ins);
 
/*execute the decoded instruction */
int execute(decoded_instruction di);
#endif
