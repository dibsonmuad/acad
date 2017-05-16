#include "L6472_spi.h"
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "drvgpio.h"

FILE* logfile;

struct spi_ioc_transfer spi_transfer[MAXTXLEN + MAXRXLEN] = {{0}};

uint8_t tx_buf[MAXTXLEN + MAXRXLEN][MAXDEVICES] = {{0}};
uint8_t rx_buf[MAXRXLEN][MAXDEVICES]            = {{0}};
uint8_t tx_msg[MAXTXLEN]                        = {0};

int g_rx_val[MAXDEVICES] = {0};

static uint8_t _parallel_transfer = 0;
static uint8_t _parallel_tx_len   = 0;
static uint8_t _parallel_rx_len   = 0;
static uint8_t ndevices           = 0;

int fdspi = 0;
#define SPOOF_TRANSFER 0
#define DEBUG_TRANSFER 0
#define DEBUG 0

/* configure this structure for spi transfer */
spidev_config config_Spidev = {.spi_bus  = "/dev/spidev1.0",
                               .speed    = 1000000,
                               .delay    = 0,
                               .bits     = 8,
                               .spi_mode = 3,
                               .ndevices = 0};

void wait_busy(void)
{
    while (get_gpio_state(GPIO_BUSY) == GPIO_LOW)
    {
        usleep(500 * 100); // sleep for 0.05s
    }

    return;
}

int spidev_configure(spidev_config* config)
{

    int ret = 0;
#if (SPOOF_TRANSFER == 0)
    fdspi = open(config->spi_bus, O_RDWR);
    if (fdspi < 0)
        pabort("can't open device");

    // spi mode
    ret = ioctl(fdspi, SPI_IOC_WR_MODE, &config->spi_mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(fdspi, SPI_IOC_RD_MODE, &config->spi_mode);
    if (ret == -1)
        pabort("can't get spi mode");

    // bits per word
    ret = ioctl(fdspi, SPI_IOC_WR_BITS_PER_WORD, &config->bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fdspi, SPI_IOC_RD_BITS_PER_WORD, &config->bits);
    if (ret == -1)
        pabort("can't get bits per word");

    // max speed hz
    ret = ioctl(fdspi, SPI_IOC_WR_MAX_SPEED_HZ, &config->speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(fdspi, SPI_IOC_RD_MAX_SPEED_HZ, &config->speed);
    if (ret == -1)
        pabort("can't get max speed hz");
#endif
    for (uint8_t i = 0; i < MAXTXLEN + MAXRXLEN; i++)
    {
        spi_transfer[i].delay_usecs   = config->delay;
        spi_transfer[i].speed_hz      = config->speed;
        spi_transfer[i].bits_per_word = config->bits;
        spi_transfer[i].cs_change     = 1;
        spi_transfer[i].tx_buf        = (unsigned long)NULL;
        spi_transfer[i].rx_buf        = (unsigned long)NULL;
    }
    if (config != &config_Spidev)
    {
        memcpy(&config_Spidev, config, sizeof(spidev_config));
    }

    ndevices = config->ndevices;

    printf("spi mode: %d\n", config->spi_mode);
    printf("spi bus: %s\n", config->spi_bus);
    printf("bits per word: %d\n", config->bits);
    printf("max speed: %d Hz (%d KHz)\n", config->speed, config->speed / 1000);
    printf("delay: %d\n", config->delay);
    printf("devices: %d\n", config->ndevices);

    return ret;
}

int spi_initialize(int ndev, char* path)
{
    if (!logfile)
        logfile = fopen("motordrv.log", "a");

    config_Spidev.ndevices = ndev;
    strncpy(&config_Spidev.spi_bus, path, sizeof(config_Spidev.spi_bus));
    spidev_configure(&config_Spidev);

    return 0;
}

int spi_finalize(void)
{
    if (logfile)
        fclose(logfile);
    close(fdspi);
}

register_entry registers[] = {{0x00, 0x00, 0, 0},
                              {0x01, ABS_POS, 22, R | WS},
                              {0x02, EL_POS, 9, R | WS},
                              {0x03, MARK, 20, R | WR},
                              {0x04, SPEED, 20, R},
                              {0x05, ACC, 12, R | WS},
                              {0x06, DEC, 12, R | WS},
                              {0x07, MAX_SPEED, 10, R | WR},
                              {0x08, MIN_SPEED, 10, R | WR},
                              {0x09, KVAL_HOLD, 8, R | WR},
                              {0x0A, KVAL_RUN, 8, R | WR},
                              {0x0B, KVAL_ACC, 8, R | WR},
                              {0x0C, KVAL_DEC, 8, R | WR},
                              {0x0D, INT_SPEED, 14, R | WH},
                              {0x0E, ST_SLP, 8, R | WH},
                              {0x0F, FN_SLP_ACC, 8, R | WH},
                              {0x10, FN_SLP_DEC, 8, R | WH},
                              {0x11, K_THERM, 4, R | WR},
                              {0x12, ADC_OUT, 5, R},
                              {0x13, OCD_TH, 4, R | WR},
                              {0x14, STALL_TH, 7, R | WR},
                              {0x15, FS_SPD, 10, R | WR},
                              {0x16, STEP_MODE, 8, R | WH},
                              {0x17, ALARM_EN, 8, R | WS},
                              {0x18, CONFIG, 16, R | WH},
                              {0x19, STATUS, 16, R}};

command_entry commands[] = {

    {NOP, "nop", 0x00, "", 1, 0},
    {SET_PARAM, "setparam", 0x00, "%d %d", -1, 0},
    {GET_PARAM, "getparam", 0x20, "%d", 1, -1},
    {RUN, "run", 0x51, "%d %d", 4, 0},
    {STEP_CLOCK, "stepclock", 0x59, "%d", 1, 0},
    {MOVE, "move", 0x41, "%d %d", 4, 0},
    {GOTO_POS, "gotopos", 0x68, "%d", 4, 0},
    {GOTO_DIR, "gotodir", 0x69, "%d %d", 4, 0},
    {GO_UNTIL, "gountil", 0x8B, "%d %d %d", 4, 0},
    {RELEASE_SW, "releasesw", 0x9B, "%d %d", 1, 0},
    {GO_HOME, "gohome", 0x70, "", 1, 0},
    {GO_MARK, "gomark", 0x78, "", 1, 0},
    {RESET_POS, "resetpos", 0xD8, "", 1, 0},
    {RESET_DEVICE, "resetdevice", 0xC0, "", 1, 0},
    {SOFT_STOP, "softstop", 0xB0, "", 1, 0},
    {HARD_STOP, "hardstop", 0xB8, "", 1, 0},
    {SOFT_HIZ, "softhiz", 0xA0, "", 1, 0},
    {HARD_HIZ, "hardhiz", 0xA8, "", 1, 0},
    {GET_STATUS, "getstatus", 0xD0, "", 1, 2},
    {RAW_CMD, "raw", 0x00, "%d", 0, 0},
    {CONFIG_MOTOR, "configmotor", 0x00, "%d", 0, 0},
    {FIND_HOME, "findhome", 0x00, "", 0, 0},

};

// Calling program is responsible for allocating data and setting up all
// support variables like ndevices, MAXTXLEN, MAXRXLEN etc
// uint8_t **tx_buf,  **rx_buf;

void pabort(const char* s)
{
    perror(s);
    abort();
}

static void pack_data(int value, uint8_t tx_len)
{
    for (uint8_t i = 1; i < tx_len; i++)
    {
#if DEBUG
        fprintf(logfile,
                "byte %d: %d\n",
                i,
                (value >> ((tx_len - 1 - i) * 8) & 0xFF));
#endif
        // printf(stdout,"byte %d: %d\n", i, (value >> ((tx_len-1 - i) * 8) &
        // 0xFF));
        tx_msg[i] = (uint8_t)(value >> ((tx_len - 1 - i) * 8) & 0xFF);
    }
}

static void setup_tx(uint8_t device, uint8_t tx_len, uint8_t rx_len)
{
    // Transmit Buf
    for (uint8_t i = 0; i < tx_len; i++)
    {
        tx_buf[i][ndevices - device] = tx_msg[i];
        spi_transfer[i].len          = ndevices;
        spi_transfer[i].tx_buf       = (unsigned long)tx_buf[i];
        spi_transfer[i].rx_buf       = (unsigned long)NULL;
#if DEBUG_TRANSFER
        fprintf(logfile, "TX: ");
        for (uint8_t ret = 0; ret < spi_transfer[i].len; ret++)
        {
            fprintf(logfile, "%.2X ", tx_buf[i][ret]);
        }
        fprintf(logfile, "\n");
#endif
    }
}

static void setup_rx(uint8_t tx_len, uint8_t rx_len)
{
    // Receive Buf
    if (rx_len > 0)
    {
        for (uint8_t i = tx_len; i < rx_len + tx_len; i++)
        {
            spi_transfer[i].len = ndevices;
            spi_transfer[i].tx_buf = (unsigned long)tx_buf[i]; // just nops
            spi_transfer[i].rx_buf =
                (unsigned long)rx_buf[i - tx_len]; //( rx_buf + (i - tx_len) );
#if DEBUG_TRANSFER
            fprintf(logfile, "TRX:");
            for (uint8_t ret = 0; ret < spi_transfer[i].len; ret++)
            {
                fprintf(logfile, "%.2X ", tx_buf[i][ret]);
            }
            fprintf(logfile, "\n");
#endif
        }
    }
}

void transfer(uint8_t device, uint8_t tx_len, uint8_t rx_len)
{
    int ret = 0, ntransfers = tx_len + rx_len;
#if DEBUG
    fprintf(logfile,
            "device: %d, tx_len: %d, rx_len: %d\n",
            device,
            tx_len,
            rx_len);
#endif

    if (!_parallel_transfer)
    {
        memset(tx_buf, 0, sizeof tx_buf);
        memset(rx_buf, 0, sizeof rx_buf);
    }
    else
    {
        if (_parallel_rx_len < rx_len)
            _parallel_rx_len = rx_len;
        if (_parallel_tx_len < tx_len)
            _parallel_tx_len = tx_len;
    }

    setup_tx(device, tx_len, rx_len);
    if (!_parallel_transfer)
    {
        setup_rx(tx_len, rx_len);
#if (SPOOF_TRANSFER == 0)
        ret = ioctl(fdspi, SPI_IOC_MESSAGE(ntransfers), &spi_transfer);
        if (ret < 1)
        {
            pabort("can't send spi message");
        }
#endif
    }
}

void begin_parallel(void)
{
    _parallel_transfer = 1;
    memset(tx_buf, 0, sizeof tx_buf);
    memset(rx_buf, 0, sizeof rx_buf);
    _parallel_tx_len = 0;
    _parallel_rx_len = 0;
#if DEBUG_TRANSFER
    fprintf(logfile, "<Parallel>\n");
#endif
    // memset(_parallel_rx_len, 0, sizeof _parallel_rx_len);
    // memset(g_rx_val, 0, sizeof g_rx_val);
}

void send_parallel(void)
{
    int ret        = 0;
    int ntransfers = _parallel_tx_len + _parallel_rx_len;
#if DEBUG_TRANSFER
    fprintf(logfile, "</Parallel>\n");
#endif
    if (ntransfers > 0)
    {
        // All tx bufs should be set up by now.
        setup_rx(_parallel_tx_len, _parallel_rx_len);
#if (SPOOF_TRANSFER == 0)
        ret = ioctl(fdspi, SPI_IOC_MESSAGE(ntransfers), &spi_transfer);
        if (ret < 1)
        {
            pabort("can't send spi message");
        }
#endif
        memset(tx_buf, 0, sizeof tx_buf);
        if (_parallel_rx_len > 0)
        {
            for (int i = 1; i <= ndevices; i++)
                g_rx_val[i] = parse_return(i, _parallel_rx_len);
        }
        memset(rx_buf, 0, sizeof rx_buf);
    }

    _parallel_transfer = 0;
    _parallel_tx_len   = 0;
    _parallel_rx_len   = 0;
}

void set_param(uint8_t device, uint8_t reg, int value)
{
    COMMAND cmd_id    = SET_PARAM;
    uint8_t param_len = 0, tx_len = 0, rx_len = commands[cmd_id].rx_len;

    tx_msg[0] =
        (uint8_t)((registers[reg].address & 0x1F) | commands[cmd_id].mask);
    // override txbytes - upto 4 bytes
    param_len = registers[reg].len;
    tx_len =
        (!(param_len % 8) ? (param_len / 8) : (param_len / 8 + 1)) /*arg len*/
        + 1 /*cmd len*/;
#if DEBUG
    fprintf(logfile,
            "param:%.2X value:%d, %0X%d\n",
            registers[reg].address,
            value,
            value);
#endif
    pack_data(value, tx_len);
    transfer(device, tx_len, rx_len);
}

int parse_return(uint8_t device, uint8_t rx_len)
{
    int value = 0;
    for (int i = 0; i < rx_len; i++)
    {
        value = (value << 8) | rx_buf[i][ndevices - device];
    }
#if DEBUG
    fprintf(logfile, "received val: %X\n", value);
#endif
    return value;
}

int get_param(uint8_t device, uint8_t param)
{
    COMMAND cmd_id = GET_PARAM;
    uint8_t tx_len = commands[cmd_id].tx_len, rx_len = 0,
            param_len = registers[param].len;
    tx_msg[0]         = (uint8_t)((param & 0x1F) | commands[cmd_id].mask);
    // Recv
    rx_len = (!(param_len % 8) ? (param_len / 8)
                               : (param_len / 8 + 1)) /*arg len only*/;
    transfer(device, tx_len, rx_len);
    if (_parallel_transfer)
        return 0;
    return parse_return(device, rx_len);
}

void run(uint8_t device, uint8_t dir, int mot_speed)
{
    COMMAND cmd_id = RUN;
    uint8_t tx_len = commands[cmd_id].tx_len;

    dir       = (dir | 0xFE);
    tx_msg[0] = (uint8_t)(dir & commands[cmd_id].mask);
    pack_data(mot_speed, tx_len);
    transfer(device, tx_len, commands[cmd_id].rx_len);
}

void step_clock(uint8_t device, uint8_t dir)
{
    COMMAND cmd_id = STEP_CLOCK;

    dir       = (dir | 0xFE);
    tx_msg[0] = (uint8_t)(dir & commands[cmd_id].mask);
    transfer(device, commands[cmd_id].tx_len, commands[cmd_id].rx_len);
}

void move(uint8_t device, uint8_t dir, float steps)
{
    COMMAND cmd_id = MOVE;
    uint8_t tx_len = commands[cmd_id].tx_len;
    int nsteps     = (int)steps;
    dir            = (dir | 0xFE);
    tx_msg[0]      = (uint8_t)(dir & commands[cmd_id].mask);
    pack_data(nsteps, tx_len);
    transfer(device, commands[cmd_id].tx_len, commands[cmd_id].rx_len);
}

void goto_pos(uint8_t device, int abs_pos)
{
    COMMAND cmd_id = GOTO_POS;
    uint8_t tx_len = commands[cmd_id].tx_len;
    tx_msg[0]      = (uint8_t)(commands[cmd_id].mask);
    pack_data(abs_pos, tx_len);
    transfer(device, tx_len, commands[cmd_id].rx_len);
}

void goto_dir(uint8_t device, uint8_t dir, int abs_pos)
{
    COMMAND cmd_id = GOTO_DIR;
    uint8_t tx_len = commands[cmd_id].tx_len;

    dir       = (dir | 0xFE);
    tx_msg[0] = (uint8_t)(dir & commands[cmd_id].mask);
    pack_data(abs_pos, tx_len);
    transfer(device, tx_len, commands[cmd_id].rx_len);
}

void go_until(uint8_t device, uint8_t act, uint8_t dir, int mot_speed)
{

    COMMAND cmd_id = GO_UNTIL;
    uint8_t tx_len = commands[cmd_id].tx_len;

    dir       = (dir | 0xFE);
    act       = ((act << 3) | 0xF7);
    tx_msg[0] = (uint8_t)(act & dir & commands[cmd_id].mask);
    pack_data(mot_speed, tx_len);
    transfer(device, tx_len, commands[cmd_id].rx_len);
}

void release_sw(uint8_t device, uint8_t act, uint8_t dir)
{
    COMMAND cmd_id = RELEASE_SW;
    uint8_t tx_len = commands[cmd_id].tx_len;

    dir       = (dir | 0xFE);
    act       = ((act << 3) | 0xF7);
    tx_msg[0] = (uint8_t)(act & dir & commands[cmd_id].mask);
    transfer(device, tx_len, commands[cmd_id].rx_len);
}

int go_home(uint8_t device) { return exec_cmd(device, GO_HOME); }

int go_mark(uint8_t device) { return exec_cmd(device, GO_MARK); }

int reset_pos(uint8_t device) { return exec_cmd(device, RESET_POS); }

int reset_device(uint8_t device) { return exec_cmd(device, RESET_DEVICE); }

int soft_stop(uint8_t device) { return exec_cmd(device, SOFT_STOP); }

int hard_stop(uint8_t device) { return exec_cmd(device, HARD_STOP); }

int soft_hiz(uint8_t device) { return exec_cmd(device, SOFT_HIZ); }

int hard_hiz(uint8_t device) { return exec_cmd(device, HARD_HIZ); }

int get_status(uint8_t device) { return exec_cmd(device, GET_STATUS); }

int exec_cmd(uint8_t device, COMMAND cmd_id)
{
    uint8_t rx_len = commands[cmd_id].rx_len;
    tx_msg[0]      = commands[cmd_id].mask;
    transfer(device, commands[cmd_id].tx_len, rx_len);
    if (_parallel_transfer)
        return 0;
    return parse_return(device, rx_len);
}

int raw_cmd(uint8_t device, uint8_t nbytes)
{
    int val        = 0;
    uint8_t tx_len = 0, rx_len = 0;
    for (uint8_t i = 0; i < nbytes; i++)
    {
        printf("byte %d: \n", i);
        scanf("%d", &val);
        tx_msg[i] = (uint8_t)val;
    }
    tx_len = rx_len = nbytes;
    transfer(device, tx_len, rx_len);
    return parse_return(device, rx_len);
}

void exec_cmd_all(COMMAND cmd_id)
{
    begin_parallel();
    {
        for (int i = 1; i <= ndevices; i++)
        {
            exec_cmd(i, cmd_id);
        }
    }
    send_parallel();
}

uint8_t get_ndevices(void) { return ndevices; }

void set_ndevices(uint8_t n) { ndevices = config_Spidev.ndevices = n; }
