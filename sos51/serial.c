#include <8051.h>
#define XOFF 0x13
#define XON 0x11

// Serial port ISR. SIO_VECTOR = 4
void serial_isr(void) using(2) interrupt SI0_VECTOR
{
    if (RI && SBUF == XOFF)
        TI = 0;
    if (RI && SBUF == XON)
        TI = 1;
    RI = 0;
}
