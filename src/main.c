/******************************************************************************
* Hopefully driving Waveshare 9.7" E-Ink                                      *
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
#include "bootloader.h"
#include "eink.h"
#include "uart.h"
#include "tps65185.h"

int main(void)
{
  uint8_t cmd = 0;
  eink_init();
  i2c_init();
  uart_init();
  tps65185_init();

  printf("Waveshare Evalualtion V1.0");

  while(1)
  {
    if(uart_checkbuffer())
    {
      cmd = uart_getc();

      if(cmd == 'x')
        bootloader_start();

      if(cmd == 's')
        tps65185_get_status();

      if(cmd == '0')
      {
        eink_powerdown();
        printf("PowerDown\r\n");
      }

      if(cmd == '1')
      {
        eink_powerup();
        printf("PowerUp\r\n");
      }
      
      if(cmd == '2')
      {
        printf("Set some pixels\r\n");
        if(eink_powerup()) 
        {
          printf("i2c error occured\r\naborteed\r\n");
          eink_powerdown();
        }
        else
        {
          printf("PowerUp\r\n");
          _delay_ms(1);
          set_pixels();
          _delay_ms(1);
          eink_powerdown();
          printf("PowerDown\r\n");
          printf("Done\r\n");
        }
      }
    }
  }
}
