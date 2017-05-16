#include "drvgpio.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <user-gpio.h>

extern void pabort(const char*);

void setup_gpio(void)
{
    if (gpio_init() < 0)
    {
        pabort("gpio_init failed");
    }
}

void close_gpio(void) { gpio_term(); }

int config_gpio(GPIO_DIR dir, int gpio_num)
{
    switch (dir)
    {
    case GPIO_INPUT:
    {
        if (gpio_direction_input(gpio_num) < 0)
            pabort("gpio_direction_input failed");
    }
    break;
    case GPIO_OUTPUT_LOW:
    {
        if (gpio_direction_output(gpio_num, 0) < 0)
            pabort("gpio_direction_output failed");
    }
    break;
    case GPIO_OUTPUT_HIGH:
    {
        if (gpio_direction_output(gpio_num, 1) < 0)
            pabort("gpio_direction_output failed");
    }
    break;
    default:
    {
        printf("GPIO_DIR:%d\n", dir);
        pabort("invalid gpio direction");
    }
    }
}

void reset_line(void)
{
    // call only after setup gpio

    return;
}

GPIO_STATE get_gpio_state(int gpio_num)
{
    return (gpio_get_value(gpio_num) == 1) ? GPIO_HIGH : GPIO_LOW;
}

void set_gpio_state(int gpio_num, GPIO_STATE state)
{
    if (gpio_get_value(gpio_num) != state)
        gpio_set_value(gpio_num, state);
}

extern int timeval_subtract(struct timeval*, struct timeval*, struct timeval*);

int wait_gpio_toggle(int gpio_num, GPIO_STATE initial, unsigned int timeout_ms)
{
    static struct timeval start, now, diff;
    gettimeofday(&start, NULL);

    while (gpio_get_value(gpio_num) == initial)
    {
        usleep(500 * 100);
        if (timeout_ms > 0)
        {
            gettimeofday(&now, NULL);
            unsigned int elapsed_ms =
                (unsigned int)((now.tv_usec - start.tv_usec) / 1000.0
                               + (now.tv_sec - start.tv_sec) * 1000.0);
            if (elapsed_ms > timeout_ms)
            {
                fprintf(
                    stderr, "elapsed:%u timeout:%u\n", elapsed_ms, timeout_ms);
                pabort("Exceeded gpio toggle timeout");
            }
        }
    }
    return 1;
}
