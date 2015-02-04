#ifndef __symtable_h__
#define __symtable_h__
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* The symbol table will be an array of structures */
/* that holds the following information

	1. Symbol - The actual symbol may be a keyword,
			identifier or constant, also things like
			'*' ',' '&' ...
*/


extern int nentries;

extern int gl_ntemps;

extern token_t* symboltable[500];

extern char* gl_prog_prefix;

/*preloads the symbol table with the symbol(?) file*/
void preload(FILE* symbol_file);

/*checks whether a token is present in the symbol table. If it does
  then it returns the id of that token. else returns 0. */
token_t* lookup(char* token);

/*inserts a token into the symbol table*/
token_t* insert(char* token_name,int type);

/*given a token_name and id, create a token of type token_t and return it*/ 
token_t* create_token(char* token_name, int type );

void print_st();

void print_token(token_t*);

token_t* gen_temp();

void assign_address();
#endif
