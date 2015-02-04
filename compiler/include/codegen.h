#ifndef __codegen__hh__
#define __codegen__hh__
#include "parser.h"
/** The following opcodes will be recognized 
1. LOAD : 
         LD reg,mem
	     The value in mem will be loaded to register reg.
These are other possible addressing modes I can think of:
 Memory Indirect ??
	 LD  reg,[mem]
             The value in mem will be loaded to reg    
 Register Indirect ??
         LD reg1,reg2
             The memory location in reg2 will be loaded to reg1

2. STORE :
         ST mem,reg
	     Store the contents of reg to memory
3. ARITHMETIC : 
         ADD/SUB/MUL/DIV :
	 eg:
         ADD reg1,reg2
	     Add the contents of reg1 and reg2 and put the result back in
	     reg1.
	 for all operations, we need additional addressing modes like
	 ADD reg,mem
4. BRANCH :
         JLT,JGT,JEQ,JMP 
	 (what about JGE,JLE? -- if JGE is true, JLT is false) 
5. IO :
         READ,WRITE,NEWLINE
6. ADDR: 
         ADDR reg1,a
         loads the mem location of variable 'a'

*/

#define OP_LD       0
#define OP_LDA      1 
/* load an immediate value into the compiler It is also used to
   load the base address of a variable/memory location
*/
#define OP_LDI      2
 
//#define OP_LDB 

#define OP_ST       3
#define OP_STA      4



#define OP_ADD       5     
#define OP_SUB       6 
#define OP_MUL       7   
#define OP_DIV       8 

#define OP_BLT        9 
#define OP_BGT       10
#define OP_BEQ       11  
#define OP_B         12

#define OP_READ      13
#define OP_WRITE     14
#define OP_NEWLINE   15


extern quad* quadtable[1000];
extern token_t* symboltable[500];


void gen_io_op(quad* q);
void gen_ar_op(quad* q);
void gen_cond_op(quad* q);
void gen_assign(quad* q);
void gen_base(quad* q);
void gen_asm(int opcode ,int op1,int op1type, int op2,int op2type);
void codegen();
void gen_branch(quad* q);
void gen_binary();
#endif
