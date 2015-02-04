#ifndef __parser__h_
#define __parser__h_
#include <stdbool.h>
#include "lex.h"
#include "stack.h"
#include "quad_table.h"
#include "types.h"
extern stack SAS;
extern int gl_nquads;
extern quad* quadtable[1000];
extern FILE* gl_error_log;
extern int gl_nerrors;
extern int gl_reading_stmts;

bool P();
bool D1();
bool D();
bool D2();
bool IL();
bool IL1();
bool S();
bool S1();
bool C1();
bool C2();
bool T();
bool T1();
bool E();
bool E1();
bool E2();
bool O();
void parse_error();

#endif
