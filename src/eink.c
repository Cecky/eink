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
#include "eink.h"

/******************************************************************************
* Configure all gpios                                                         *
******************************************************************************/
void eink_init(void)
{
  //setup gpio
  DDR_XCL |= (1<<XCL);
  DDR_XLE |= (1<<XLE);
  DDR_XOE |= (1<<XOE);
  DDR_XSTL |= (1<<XSTL);
  DDR_MODE1 |= (1<<MODE1);
  DDR_MODE2 |= (1<<MODE2);
  DDR_SPV |= (1<<SPV);
  DDR_CKV |= (1<<CKV);

  DDR_D0 |= (1<<D0);
  DDR_D1 |= (1<<D1);
  DDR_D2 |= (1<<D2);
  DDR_D3 |= (1<<D3);
  DDR_D4 |= (1<<D4);
  DDR_D5 |= (1<<D5);
  DDR_D6 |= (1<<D6);
  DDR_D7 |= (1<<D7);

  CLR_XLE;
  CLR_XOE;
  CLR_XCL;
  CLR_XSTL;
  eink_set_data(0);
  CLR_CKV;
  CLR_MODE;
}

/******************************************************************************
* Write 1 databyte to the corresponding data-pins                             *
******************************************************************************/
void eink_set_data(uint8_t data)
{
#ifdef USE_FULL_PORT
  PORT_FULL = data;
#else
  if(data & 0x01) PORT_DO |= (1<<D0);
  else PORT_D0 &= ~(1<<D0);

  if(data & 0x02) PORT_D1 |= (1<<D1);
  else PORT_D1 &= ~(1<<D1);

  if(data & 0x04) PORT_D2 |= (1<<D2);
  else PORT_D2 &= ~(1<<D2);

  if(data & 0x08) PORT_D3 |= (1<<D3);
  else PORT_D3 &= ~(1<<D3);

  if(data & 0x10) PORT_D4 |= (1<<D4);
  else PORT_D4 &= ~(1<<D4);

  if(data & 0x20) PORT_D5 |= (1<<D5);
  else PORT_D5 &= ~(1<<D5);

  if(data & 0x40) PORT_D6 |= (1<<D6);
  else PORT_D6 &= ~(1<<D6);

  if(data & 0x80) PORT_D7 |= (1<<D7);
  else PORT_D7 &= ~(1<<D7);
#endif
}

/******************************************************************************
* Powerup all rails in the correct order                                      *
* return:  0    succes                                                        *
*          -1   TPS65185 is not responding                                    *
******************************************************************************/
int8_t eink_powerup(void)
{
  //set control lines
  CLR_XLE;
  CLR_XOE;
  CLR_XCL;
  SET_XSTL;
  eink_set_data(0);
  CLR_CKV;
  CLR_MODE;

  _delay_us(100);
  
  if(!tps65185_write(UPSEQ0, WAVESHARE97UP))
    TPS_POWERUP;
  else
    return -1;
  
  // no error
  return 0;
}

/******************************************************************************
* Powerdown all rails in the correct order                                    *
******************************************************************************/
void eink_powerdown(void)
{
  CLR_XLE;
  CLR_XOE;
  CLR_XCL;
  SET_XSTL;
  eink_set_data(0);
  CLR_CKV;
  CLR_MODE;

  TPS_POWERDOWN;
}

/******************************************************************************
* Fast vertical clock pulse for gate driver, used during initializations      *
******************************************************************************/
void vclock_quick()
{
  SET_CKV;
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  CLR_CKV;
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop;");
}

/******************************************************************************
* Horizontal clock pulse for clocking data into source driver                 *
******************************************************************************/
void hclock()
{
  //asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  SET_XCL;
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  CLR_XCL;
}

/******************************************************************************
* Start a new vertical gate driver scan from top.                             *
* Note: Does not clear any previous bits in the shift register,               *
*       so you should always scan through the whole display before            *
*       starting a new scan.                                                  *
******************************************************************************/
void vscan_start()
{
    SET_MODE;
    vclock_quick();
    CLR_SPV;
    vclock_quick();
    SET_SPV;
    vclock_quick();
}

/******************************************************************************
* Waveform for strobing a row of data onto the display.                       *
* Attempts to minimize the leaking of color to other rows by having           *
* a long idle period after a medium-length strobe period.                     *
******************************************************************************/
void vscan_write()
{
  SET_CKV;
  SET_XOE;
  // 5us delay
   //clock_delay_usec(20);
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop;");
  CLR_XOE;
  CLR_CKV;
  // 200us delay
  _delay_us(183);
}

/******************************************************************************
* Waveform used when clearing the display. Strobes a row of data to the       *
* screen, but does not mind some of it leaking to other rows.                 *
******************************************************************************/
void vscan_bulkwrite()
{
  SET_CKV;
  _delay_us(15);
  CLR_CKV;
  _delay_us(195);
}

/******************************************************************************
* Waveform for skipping a vertical row without writing anything.              *
* Attempts to minimize the amount of change in any row.                       *
******************************************************************************/
void vscan_skip()
{
  SET_CKV;
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  CLR_CKV;
  _delay_us(97);
}

/******************************************************************************
* Stop the vertical scan. The significance of this escapes me, but it seems   *
* necessary or the next vertical scan may be corrupted.                       *
******************************************************************************/
void vscan_stop()
{
  CLR_MODE;
  vclock_quick();
  vclock_quick();
  vclock_quick();
  vclock_quick();
  vclock_quick();
}

/******************************************************************************
* Start updating the source driver data (from left to right).                 *
******************************************************************************/
void hscan_start()
{
  /* Disable latching and output enable while we are modifying the row. */
  CLR_XLE;
  CLR_XOE;

  /* The start pulse should remain low for the duration of the row. */
  CLR_XSTL;
}

/******************************************************************************
* Write data to the horizontal row.                                           *
******************************************************************************/
void hscan_write(const uint8_t *data, int count)
{
  while (count--)
  {
    /* Set the next byte on the data pins */
    eink_set_data(*data++);

    /* Give a clock pulse to the shift register */
    hclock();
  }
}

/******************************************************************************
* Finish and transfer the row to the source drivers.                          *
* Does not set the output enable, so the drivers are not yet active.          *
******************************************************************************/
void hscan_stop()
{
  SET_XSTL;
  // 1us delay
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  SET_XCL;
  // 1us delay
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  CLR_XCL;
  SET_XLE;
  // 1us delay
  asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; \
       nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
  CLR_XLE;
}
