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
  SET_XOE;
  CLR_XCL;
  SET_XSTL;
  eink_set_data(0);
  CLR_CKV;
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
*                                                                             *
* note: doesn't look similar to the logicanalyser readout but works fine.     *
*       further investigation needed!                                         *
******************************************************************************/
void eink_send_row(uint8_t *data)
{
  SET_XLE; 
  CLR_XLE;
  SET_XOE;
  CLR_XSTL;                                          
  
  for (uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
  {
    eink_set_data(data[i]);
    SET_XCL;
    CLR_XCL;
  }
  
  SET_XSTL;
  CLR_CKV;
  CLR_XOE;
  SET_CKV;     
}

/******************************************************************************
* start a new vertical gate driver scan from top.                             *
******************************************************************************/
void eink_start_scan(void)
{
  SET_CKV;
  _delay_us(5);
  CLR_SPV;
  _delay_us(5);
  CLR_CKV;

  _delay_us(3);
  SET_CKV;
  _delay_us(5);
  SET_SPV;
  _delay_us(5);
  CLR_CKV;

  _delay_us(3);
  SET_CKV;
  _delay_us(11);
  CLR_CKV;
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
* write sync-frame                                                            *
*                                                                             *
* note: only immediately after the syncframe the following                    *
* frames start from the top                                                   *
******************************************************************************/
void eink_sync(void)
{
  eink_start_scan();
  for(uint16_t line = 0; line < SCREEN_HEIGHT; line++)
  {
    for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
    {
      line_data[i] = 0;
    }
    eink_send_row(line_data);
  }
}

/******************************************************************************
* draw linepattern                                                            *
******************************************************************************/
void eink_draw_line(uint8_t clear)
{
  eink_sync();

  if(clear)
  {
    eink_clear();
  }

  for(uint8_t frame = 0; frame < 4; frame++)
  {
    eink_start_scan();
    for(uint16_t line = 0; line < SCREEN_HEIGHT; line++)
    {
      for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
      {
        if(!(line % 100)) 
          //line_data[i] = BLACK;
          line_data[i] = DOTTED;

        else
          line_data[i] = 0;
      }
      eink_send_row(line_data);
    }
  }
}

/******************************************************************************
* generate a small checkerboard at specific location                          *
******************************************************************************/
void eink_checkerboard(uint16_t x, uint16_t y)
{
  uint8_t cnt = 0;
  uint8_t cb_linepattern_1[SCREEN_WIDTH / 4];
  uint8_t cb_linepattern_2[SCREEN_WIDTH / 4];

  //Set values
  for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
  {
    cb_linepattern_1[i] = WHITE;
    cb_linepattern_2[i] = WHITE;
  }

  cb_linepattern_1[100] = BLACK;
  cb_linepattern_1[101] = BLACK;
  cb_linepattern_1[104] = BLACK;
  cb_linepattern_1[105] = BLACK;
  cb_linepattern_1[108] = BLACK;
  cb_linepattern_1[109] = BLACK;
  cb_linepattern_1[112] = BLACK;
  cb_linepattern_1[113] = BLACK;

  cb_linepattern_2[102] = BLACK;
  cb_linepattern_2[103] = BLACK;
  cb_linepattern_2[106] = BLACK;
  cb_linepattern_2[107] = BLACK;
  cb_linepattern_2[110] = BLACK;
  cb_linepattern_2[111] = BLACK;
  cb_linepattern_2[114] = BLACK;
  cb_linepattern_2[115] = BLACK;

  eink_sync();
  for(uint8_t frame = 0; frame < 4; frame++)
  {
    cnt = 0;
    eink_start_scan();
    for(uint16_t line = 0; line < SCREEN_HEIGHT; line++)
    {
      for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
      {
        if((line >= y) && (line < (y + 64)))
        {
          if( (cnt/8)%2 )
            line_data[i] = cb_linepattern_1[i];
          else
            line_data[i] = cb_linepattern_2[i];
        }
        else
          line_data[i] = 0;
      }
      if((line >= y) && (line < (y + 64))) cnt++;
      eink_send_row(line_data);
    }
  }
}

/******************************************************************************
* print character at a specific coordinate (anker = top-left)                 *
*                                                                             *
* for testing used 5x8 font. works but it's definetly to small!               *
******************************************************************************/
void eink_print_char(uint16_t x, uint16_t y, uint8_t c)
{
  uint16_t data_byte = 0;
  uint8_t data_bit = 0;
  uint8_t line_cnt = 0;
  uint8_t data = 0;
  uint16_t x_pos = 0;

  eink_sync();
  for(uint8_t frame = 0; frame < 4; frame++)
  {
    line_cnt = 0;
    eink_start_scan();

    for(uint16_t line = 0; line < SCREEN_HEIGHT; line++)
    {
      x_pos = x;

      // clear linebuffer
      for(uint16_t i = 0; i < SCREEN_WIDTH / 4; i++)
      {
        line_data[i] = 0;
      }

      if((line >= y) && (line < (y + FONT_HEIGHT)))
      {
        data = pgm_read_byte(&font_PGM[(c-FONT_START)*(8*FONT_HEIGHT/8)+line_cnt]);

        for(uint8_t i = 0; i < FONT_WIDTH; i++)
        {
          data_byte = x_pos / 4;
          data_bit = (x_pos % 4) * 2;

          if(data & (0x10 >> i))
          {
            line_data[data_byte] |= (0x40 >> data_bit);
          }

          x_pos++;
        }
      
        line_cnt++;
      }
      
      eink_send_row(line_data);
    }
  }
}
