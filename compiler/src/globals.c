
#include <stdio.h>
#include "types.h"
#include "stack.h"

int nentries;//no. of symbol table entries
token_t* symboltable[500];

quad* quadtable[1000];
int gl_nquads;

//line no in the input program
int gl_lineno;

//I wont be using this at all.. just to check if ungetc works right
//int gl_filepos;

//Input program;
FILE* gl_prog_stream;

//Error log file
FILE* gl_error_log;

token_t* gl_nexttoken;

//The semantic action stack
stack SAS;

//name of program file without extension
char* gl_prog_prefix;

// a count of the number of temporary vars generated
int gl_ntemps;

//if lexical error found flag this
int gl_lex_error;

//total number of errors
int gl_nerrors;

//If I have started reading statements, I set this to 1
int gl_reading_stmts;
