#include <avr/io.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "uart.h"

#define ADRESSE             0xD0

#define PORT_PWRUP          PORTC
#define DDR_PWRUP           DDRC
#define PIN_PWRUP           PC2
#define TPS_POWERUP         (PORT_PWRUP |= (1<<PIN_PWRUP))
#define TPS_POWERDOWN       (PORT_PWRUP &= ~(1<<PIN_PWRUP))

//Register Map
#define TMST_VALUE          0x00
#define ENABLE              0x01
#define VADJ                0x02
#define VCOM1               0x03
#define VCOM2               0x04
#define INT_EN1             0x05
#define INT_EN2             0x06
#define INT_1               0x07
#define INT_2               0x08
#define UPSEQ0              0x09
#define UPSEQ1              0x0A
#define DWNSEQ0             0x0B
#define DWNSEQ1             0x0C
#define TMST1               0x0D
#define TMST2               0x0E
#define PG                  0x0F
#define REVID               0x10

//powerup/down - sequence waveshare 9.7"
#define WAVESHARE97UP       0xE1
#define WAVESHARE97DOWN     0x1E


void tps65185_init(void);
int8_t tps65185_write(uint8_t reg, uint8_t val);



void tps65185_get_status(void);
