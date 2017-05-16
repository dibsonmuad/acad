#ifndef __MOTDRVAPI__
#define __MOTDRVAPI__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/** The maximum number of bytes in a transmit message */
#define MAXTXLEN 4
/** The maximum number of bytes in a receive message */
#define MAXRXLEN 3
/** The maximum number of daisy chained spi devices supported in this
 * implementation */
#define MAXDEVICES 16

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/** The list of register names that can be set or read from */
typedef enum {
    FIRST_REG = 0x01,
    ABS_POS   = FIRST_REG, // Current position
    EL_POS,                // Electrical position
    MARK,                  // Mark positon
    SPEED,
    ACC,
    DEC,
    MAX_SPEED,
    MIN_SPEED,
    KVAL_HOLD,
    KVAL_RUN = 0x0A,
    KVAL_ACC,
    KVAL_DEC,
    INT_SPEED,
    ST_SLP,
    FN_SLP_ACC,
    FN_SLP_DEC = 0x10,
    K_THERM,
    ADC_OUT,
    OCD_TH,
    STALL_TH,
    FS_SPD,
    STEP_MODE,
    ALARM_EN,
    CONFIG,
    STATUS   = 0x19,
    LAST_REG = STATUS
} REG_NAME;

/** The modes in which these registers can be Read or Written to */
typedef enum {
    R  = 1, /* Read */
    WR = 2, /* Write Always */
    WS = 4, /* Write only when motor is stopped */
    WH = 8  /* Write only when outputs are in high impedance */
} REG_TYPE;

/** The set of commands that can be sent to the ST Micro L647X class of motor
 * controllers */
typedef enum {
    NOP = 0,
    SET_PARAM,
    GET_PARAM,
    RUN,
    STEP_CLOCK,
    MOVE,
    GOTO_POS,
    GOTO_DIR,
    GO_UNTIL,
    RELEASE_SW,
    GO_HOME, // 10
    GO_MARK,
    RESET_POS,
    RESET_DEVICE,
    SOFT_STOP,
    HARD_STOP,
    SOFT_HIZ,
    HARD_HIZ,
    GET_STATUS,
    RAW_CMD,
    CONFIG_MOTOR, // 20
    DOG_PONY,
    FIND_HOME,
    FIND_HOME_PARALLEL,
    SETUP_GPIO,
    READ_BUSY,
    RESET_LINE,
    TEST_GENERIC,
    TEST_GPIO,
    TEST_GRIPPER,
    TEST_HANDOFF,
    INIT_SEQ,
    RESET_ALL,
    CONFIG_ALL,
    PRINT_LABEL,
    DUMP_REGISTERS,
    DIFF_REGISTERS,
    HELP,
    EXIT,
    LAST_COMMAND
} COMMAND;

typedef enum { EL_POS_MICROSTEP = 0x007F, EL_POS_STEP = 0x0180 } REG_EL_POS;

typedef enum {
    MIN_SPEED_MINIMUM  = 0x07FF,
    MIN_SPEED_LSPD_OPT = 0x0800
} REG_MIN_SPEED;

typedef enum {
    STEP_SEL      = 0x07,
    STEP_WR_MODE  = 0x08,
    STEP_SYNC_SEL = 0x70,
    STEP_SYNC_EN  = 0x80
} REG_STEP_MODE;

/** This structure is used to compose a spi message.
 */
typedef struct
{
    uint8_t address; /** The address tells the controller the actual register
                        that is read or written to. */
    REG_NAME name;   /** REG_NAME is described above and makes register addresses
                        human readable. */
    uint8_t len;     /** Length of the register in bits. */
    REG_TYPE type;   /** REG_TYPE is described above, but it is currently not
                        utilized */
} register_entry;

/** This structure is also used to compose a spi message in conjunction with
 * register_entry.
 */
typedef struct
{
    COMMAND id;      /** The enum referring the command id */
    const char* str; /** Human readable name for the command. */
    uint8_t
        mask;            /** Helps setup the actual command code expected by the motor
                            controller. */
    const char* fmt_str; /** */
    uint8_t tx_len;
    uint8_t rx_len;
} command_entry;

/** Use this enum to specify motor's rotation direction. */
typedef enum { REV_DIR = 0, FWD_DIR = 1 } MOTOR_DIR;

/** Used to configure spidev
 * Populate an instance of this structure before invoking
 * spidev_configure and passing the instance as a parameter.
 */
typedef struct
{
    char spi_bus[15];
    uint32_t speed;
    uint16_t delay;
    uint8_t bits;
    uint8_t spi_mode;
    uint8_t ndevices;
} spidev_config;

/** This instance of spidev_config can be queried to find out
 * spidev settings.
 */
extern spidev_config config_Spidev;

/** Wait busy will keep polling the busy flag (active low) and returns when
 * it goes back to high. This flag is held low by the motor controller until
 * it completes executing a command
 */
void wait_busy(void);

/** Invoke this function to print system error along with custom string.
 */
void pabort(const char* s);
/** Invoke this function to initialize the spi driver. */
int spi_initialize(int n, char* spi_busy);
/** Invoke this function at the end of the program to clean up. */
int spi_finalize(void);
/** This function can be directly invoked after populating a spidev_config
 * struct or is invoked internally from spi_initialize. */
int spidev_configure(spidev_config* config);

/** This function is responsible for actually sending the message over the spi
 *  daisy chain.
 */
void transfer(uint8_t device, uint8_t tx_len, uint8_t rx_len);

void set_param(uint8_t device, uint8_t reg, int value);
int parse_return(uint8_t device, uint8_t rx_len);
int get_param(uint8_t device, uint8_t param);
void run(uint8_t device, uint8_t dir, int mot_speed);
void step_clock(uint8_t device, uint8_t dir);
void move(uint8_t device, uint8_t dir, float steps);
void goto_pos(uint8_t device, int abs_pos);
void goto_dir(uint8_t device, uint8_t dir, int abs_pos);
void go_until(uint8_t device, uint8_t act, uint8_t dir, int mot_speed);
void release_sw(uint8_t device, uint8_t act, uint8_t dir);
int exec_cmd(uint8_t device, COMMAND cmd_id);
int raw_cmd(uint8_t device, uint8_t nbytes);
void exec_cmd_all(COMMAND cmd_id);
void begin_parallel(void);
void send_parallel(void);

int go_home(uint8_t device);
int go_mark(uint8_t device);
int reset_pos(uint8_t device);
int reset_device(uint8_t device);
int soft_stop(uint8_t device);
int hard_stop(uint8_t device);
int soft_hiz(uint8_t device);
int hard_hiz(uint8_t device);
int get_status(uint8_t device);

uint8_t get_ndevices(void);
void set_ndevices(uint8_t n);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __MOTDRVAPI__ */
