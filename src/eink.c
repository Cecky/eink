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
* some global variables                                                       *
******************************************************************************/
uint8_t line_data[SCREEN_WIDTH / 4];      //line data buffer
const uint8_t wave_init[FRAME_INIT_LEN]=
{
  BLACK,BLACK,BLACK,BLACK,BLACK,
  WHITE,WHITE,WHITE,WHITE,WHITE,
  BLACK,BLACK,BLACK,BLACK,BLACK,
  WHITE,WHITE,WHITE,WHITE,WHITE,
  0,
};

/******************************************************************************
* configure all gpios                                                         *
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
  SET_MODE;

  for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
  {
    line_data[i] = 0;
  }
}

/******************************************************************************
* write 1 databyte to the corresponding data-pins                             *
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
* powerup all rails in the correct order                                      *
* return:  0    succes                                                        *
*          -1   TPS65185 is not responding                                    *
******************************************************************************/
int8_t eink_powerup(void)
{
  if(tps65185_write(VCOM1, 0x04))
    return -1;
  if(tps65185_write(VCOM2, 0x01))
    return -1;

  if(!tps65185_write(UPSEQ0, WAVESHARE97UP))
    TPS_POWERUP;
  else
    return -1;

  _delay_us(100);
  
  //set control lines
  CLR_XLE;
  CLR_XOE;
  CLR_XCL;
  SET_XSTL;
  eink_set_data(0);
  SET_CKV;
  SET_SPV;
  SET_MODE;

  _delay_ms(1);
  VCOM_ON;
  
  // no error
  return 0;
}

/******************************************************************************
* powerdown all rails in the correct order                                    *
******************************************************************************/
void eink_powerdown(void)
{
  CLR_XLE;
  CLR_XOE;
  CLR_XCL;
  CLR_XSTL;
  eink_set_data(0);
  CLR_CKV;
  CLR_SPV;
  SET_MODE;

  VCOM_OFF;
  _delay_ms(1);

  TPS_POWERDOWN;
}

/******************************************************************************
* send datarow to display                                                     *
******************************************************************************/
void eink_send_row(uint8_t *data)
{
  SET_XLE; 
  SET_XCL;
  CLR_XCL;
  SET_XCL;
  CLR_XCL;

  CLR_XLE;
  SET_XCL;
  CLR_XCL;
  SET_XCL;
  CLR_XCL;
  
  SET_XOE;
  
  CLR_XSTL;                                          
  
  for (uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
  {
    eink_set_data(data[i]);
    SET_XCL;
    CLR_XCL;
  }
  
  SET_XSTL;
  
  SET_XCL;
  CLR_XCL;
  SET_XCL;
  CLR_XCL;

  CLR_CKV;
  CLR_XOE;
  
  SET_XCL;
  CLR_XCL;
  SET_XCL;
  CLR_XCL;
  
  SET_CKV;     
}

/******************************************************************************
* fast vertical clock pulse for gate driver, used during initializations      *
******************************************************************************/
void eink_vclock_quick(void)
{
  for (uint8_t i = 0; i < 2; i++)
  {
    CLR_CKV;
    SET_CKV;
  }
}

/******************************************************************************
* start a new vertical gate driver scan from top.                             *
******************************************************************************/
void eink_start_scan(void)
{ 
  eink_vclock_quick();
  CLR_SPV;
  eink_vclock_quick();
  SET_SPV;
  eink_vclock_quick();
}

/******************************************************************************
* clear display (Black<->White)                                               *
******************************************************************************/
void eink_clear(void)
{
  for(uint8_t frame = 0; frame < FRAME_INIT_LEN; frame++)			
  {
    eink_start_scan();
    for(uint16_t line = 0; line < SCREEN_HEIGHT; line++)
    {
      for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
      {
        line_data[i] = wave_init[frame];
      }
      eink_send_row(line_data);
    }
  }
}

/******************************************************************************
* draw linepattern                                                            *
*                                                                             *
* note: for some reason only after the clear cycle the linepattern starts     *
*       from the top.                                                         *
******************************************************************************/
void eink_draw_line()
{
  eink_clear();
  for(uint8_t frame = 0; frame < 4; frame++)
  {
    eink_start_scan();
    for(uint16_t line = 0; line < SCREEN_HEIGHT; line++)
    {
      for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
      {
        if(!(line % 100)) 
          line_data[i] = BLACK;
        else
          line_data[i] = WHITE;
      }
      eink_send_row(line_data);
    }
  }
}
