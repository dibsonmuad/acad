#ifndef __MOTDRVGPIO__
#define __MOTDRVGPIO__

#ifdef __cplusplus
extern "C" {
#endif

/** Possible GPIO states.
 * GPIO_LOW is mapped to 0 and GPIO_HIGH is set to 1.
 * No other GPIO_STATE is recognized
 */
typedef enum {
    GPIO_LOW  = 0,
    GPIO_HIGH = 1,
} GPIO_STATE;

/** Enum for configuring GPIO pins.
 * Used when configuring a GPIO pin for input or output.
 */
typedef enum { GPIO_INPUT, GPIO_OUTPUT_LOW, GPIO_OUTPUT_HIGH } GPIO_DIR;

/** Acts like a constructor and should be invoked before using any other
 * functions in this module This functions just initializes the underlying gpio
 * kernel module.
 */
void setup_gpio(void);

void reset_line(void);
/** Acts like a destructor and should be invoked before exiting the program.
 * This does not reset any modified GPIO lines to their original settings.
 * However, it performs some cleanup functionality.
 */
void close_gpio(void);
/** To configure a GPIO line, the direction and number should be provided. The
 * direction is an enum value described above. gpio_num is the actual pin number
 * of the GPIO pin. A note on SoCs similar to the Beaglebone. For the
 * Beaglebone, there are 4 GPIO banks numbered 0 - 3. There are 32 pins in each
 * bank. So GPIO0_12 in the schematic corresponds to gpio_num 0 * 32 + 12 = 12.
 * GPIO3_19 corresponds to gpio_num 3 * 32 + 19 = 115
 */
int config_gpio(GPIO_DIR dir, int gpio_num);

/** Returns the state of a GPIO pin. The GPIO_STATE enum is described above */
GPIO_STATE get_gpio_state(int gpio_num);

/** Sets the GPIO_STATE given a gpio_num and a state */
void set_gpio_state(int gpio_num, GPIO_STATE state);

/** Wait for a gpio to toggle from one state to another. Waits indefinitely if
 * timeout_ms is 0
 */
int wait_gpio_toggle(int gpio_num, GPIO_STATE initial, unsigned int timeout_ms);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* __MOTDRVGPIO__*/
