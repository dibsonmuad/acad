############################################################
#
# Makefile for different stages of the compiler
#
# 
############################################################
#include Make.defines

TARGET = sc

SRCS= :.c

#OBJS = $(SRCS:.c=.o) 
CC = gcc

INCDIR = -I../include

CFLAGS = -g -Wall -std=c99 $(INCDIR)

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) globals.c stack.c symbol_table.c quad_table.c lex.c parser.c main.c codegen.c

.SUFFIXES : .o .c

.c.o :
	$(CC) -c $< $(INCDIR)

clean :
	-rm -rf  *~ *.o core *.qds *.stb *.err $(TARGET)
