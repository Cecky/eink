/******************************************************************************
* Interfacing a Waveshare eink-display 9.7" with AVR                          *
*                                                                             *
* used hardware:                                                              *
* ATmega324a @14.745600 MHz                                                   *
*                                                                             *
* Copyright (C) 2019  Cecky                                                   *
*                                                                             *
* This program is free software: you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation, either version 3 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
******************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "tps65185.h"

// dimensions
#define SCREEN_HEIGHT   825
#define SCREEN_WIDTH    1200

// commandlines
#define PORT_XCL    PORTA
#define DDR_XCL     DDRA
#define XCL         PA0
#define SET_XCL     PORT_XCL |= (1<<XCL)
#define CLR_XCL     PORT_XCL &= ~(1<<XCL)

#define PORT_XLE    PORTA
#define DDR_XLE     DDRA
#define XLE         PA1
#define SET_XLE     PORT_XLE |= (1<<XLE)
#define CLR_XLE     PORT_XLE &= ~(1<<XLE)

#define PORT_XOE    PORTA
#define DDR_XOE     DDRA
#define XOE         PA2
#define SET_XOE     PORT_XOE |= (1<<XOE)
//#define CLR_XOE     PORT_XOE &= ~(1<<XOE)
#define CLR_XOE     PORT_XOE |= (1<<XOE)

#define PORT_XSTL   PORTA
#define DDR_XSTL    DDRA
#define XSTL        PA3
#define SET_XSTL    PORT_XSTL |= (1<<XSTL)
#define CLR_XSTL    PORT_XSTL &= ~(1<<XSTL)

#define PORT_MODE1  PORTA
#define DDR_MODE1   DDRA
#define MODE1       PA4
#define SET_MODE1   PORT_MODE1 |= (1<<MODE1)
#define CLR_MODE1   PORT_MODE1 &= ~(1<<MODE1)

#define PORT_MODE2  PORTA
#define DDR_MODE2   DDRA
#define MODE2       PA5
#define SET_MODE2   PORT_MODE2 |= (1<<MODE2)
#define CLR_MODE2   PORT_MODE2 &= ~(1<<MODE2)

#define SET_MODE    { SET_MODE1; SET_MODE2; }
#define CLR_MODE    { CLR_MODE1; CLR_MODE2; }

#define PORT_SPV    PORTA
#define DDR_SPV     DDRA
#define SPV         PA6
#define SET_SPV     PORT_SPV |= (1<<SPV)
#define CLR_SPV     PORT_SPV &= ~(1<<SPV)

#define PORT_CKV    PORTA
#define DDR_CKV     DDRA
#define CKV         PA7
#define SET_CKV     PORT_CKV |= (1<<CKV)
#define CLR_CKV     PORT_CKV &= ~(1<<CKV)

// datalines
#define USE_FULL_PORT

#define PORT_FULL   PORTB

#define PORT_D0     PORTB
#define DDR_D0      DDRB
#define D0          PB0

#define PORT_D1     PORTB
#define DDR_D1      DDRB
#define D1          PB1

#define PORT_D2     PORTB
#define DDR_D2      DDRB
#define D2          PB2

#define PORT_D3     PORTB
#define DDR_D3      DDRB
#define D3          PB3

#define PORT_D4     PORTB
#define DDR_D4      DDRB
#define D4          PB4

#define PORT_D5     PORTB
#define DDR_D5      DDRB
#define D5          PB5

#define PORT_D6     PORTB
#define DDR_D6      DDRB
#define D6          PB6

#define PORT_D7     PORTB
#define DDR_D7      DDRB
#define D7          PB7

// prototypes
void eink_init(void);
int8_t eink_powerup(void);
void eink_powerdown(void);
void eink_set_data(uint8_t data);


// streetsign-project stuff

/* Fast vertical clock pulse for gate driver, used during initializations */
void vclock_quick();

/* Horizontal clock pulse for clocking data into source driver */
void hclock();

/** Start a new vertical gate driver scan from top.
 * Note: Does not clear any previous bits in the shift register,
 *       so you should always scan through the whole display before
 *       starting a new scan.
 */
void vscan_start();

/* Waveform for strobing a row of data onto the display.
 * Attempts to minimize the leaking of color to other rows by having
 * a long idle period after a medium-length strobe period.
 */
void vscan_write();

/* Waveform used when clearing the display. Strobes a row of data to the
 * screen, but does not mind some of it leaking to other rows.
 */
void vscan_bulkwrite();

/* Waveform for skipping a vertical row without writing anything.
 * Attempts to minimize the amount of change in any row.
 */
void vscan_skip();

/* Stop the vertical scan. The significance of this escapes me, but it seems
 * necessary or the next vertical scan may be corrupted.
 */
void vscan_stop();

/* Start updating the source driver data (from left to right). */
void hscan_start();

/* Write data to the horizontal row. */
void hscan_write(const uint8_t *data, int count);

/* Finish and transfer the row to the source drivers.
 * Does not set the output enable, so the drivers are not yet active. */
void hscan_stop();






void set_pixels();
