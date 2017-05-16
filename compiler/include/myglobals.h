#ifndef __myglobals_h__
#define __myglobals_h__

/* this will be the basic token in the symbol_table.will be extended
later with more attributes */

#define PROGRAM 1
#define ARRAY 2
#define INTEGER 3
#define READ 4
#define WRITE 5
#define READLN 6
#define WRITELN 7
#define WHILE 8
#define DO 9
#define OD 10
#define FOR 11
#define BEGIN 12
#define END 13
#define IF 14
#define THEN 15
#define ELSE 16
#define FI 17
#define TO 18
#define FSLASH 19
#define COLON 20
#define PCNT 21
#define LSB 22
#define RSB 23
#define SCOLON 24
#define COMMA 25
#define LP 26
#define RP 27
#define ASSGN 28
#define LT 29
#define GT 30
#define EQ 31
#define LTE 32
#define GTE 33
#define NE 34
#define PLUS 35
#define MINUS 36
#define MUL 37
#define EOI 38
#define ID 39
#define CONS 40

typedef struct
{
    char* name;
    int type;
} token_t;

#endif
