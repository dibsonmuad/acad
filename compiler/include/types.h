#ifndef __myglobals__h__
#define __myglobals__h__

#include "tokens.h"
#include "quads.h"

#define sizeint 1
 
typedef struct {
  int index;//Entry no. in the symbol table. 
  char* name;
  //ID, CONS , PROGRAM .. parser uses this extensively
  //basically a numeric id for each keyword,symbol,id and cons in the program
  int type;
  //can be integer or array
  int data_type;
  //a datatype can have some specific characterestics foe eg. this will be
  //cast as a pointer to an array if data_type turns out to be an array
  void* dtype_specifics;

  //to find out if addressing is direct or indirect
  //if mode is 0 then direct, if 1 it is indirect (well almost...)
  //this is used in conjunction with a mode bit for the quad to check
  //if there is any kind of indirect addressing for that quad
  int mode;
  //memory location while generating code
  int start_address;
  
} token_t;

typedef struct {
  int number;
  int q_type;
  int op1;
  int op2;
  int result;
  short int q_mode;
  /* every quad corresponds to an assembler code no.
     this is very helpful during code generation esp. jumps
  */
  int inst_no;
} quad;

typedef struct {
  // supposed to be the starting address of the array
  int address;//machine dependent I think 
  int size;
  //max number of dimensions 
  //should change this into a linked list later to support gazillion dim arrays
  int dims[10];
  //if dim = 3 it is a 3 dimensional array
  int dim;
  
} array_t;

typedef struct {
  int opcode;
  int op1;
  int op1type;
  int op2;
  int op2type;
  /* its own index in the table */
  int inst_no;
}asm_t;
#endif
