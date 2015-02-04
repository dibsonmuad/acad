#ifndef __quad_table__hh__
#define __quad_table__hh__

#include "quads.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern token_t* symboltable[500];
extern quad* quadtable[1000];
extern int gl_nquads;
extern char* gl_prog_prefix;


void gen_quad(int q_type,int op1,int op2,int res);
char* gen_readable_quad(quad* pq);
void print_quad_table();
#endif
