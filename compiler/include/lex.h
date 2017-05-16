
#ifndef lex_hh_
#define lex_hh_

#include <ctype.h>
#include <stdio.h>
#include <string.h>
//#include "myglobals.h"
#include "symbol_table.h"
#include "types.h"

int recover();
void get_token();

extern int gl_lineno;
extern FILE* gl_prog_stream;
extern token_t* gl_nexttoken;
extern int gl_lex_error;
extern FILE* gl_error_log;
extern int gl_reading_stmts;
extern int gl_nerrors;
#endif
