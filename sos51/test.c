//! test.c preliminary unit test framework.
#include "SOS.h"
#include <assert.h>

/* Test function pointer */
bit (*tf)(void) reentrant;

/* Another Test function pointer representing a class of test functions*/
bit (*tSfrVal)(sfr, unsigned char);

bit (*tXDVal)(xdata char* address, char value);
//Test if some sfr has a particular value
bit testSfrContent(sfr s, unsigned char value)
{
	return (DEREF(s) == value);
}

//Test value at a particular external RAM address
bit testXDataContent(xdata char* address, char value)
{
	return (*address == value);
}

