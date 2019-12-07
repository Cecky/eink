/******************************************************************************
* uart.c : UART handling                                                      *
*                                                                             *
* Copyright (C) 2014  Cecky                                                   *
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
#include "uart.h"

#ifdef USE_PRINTF_TO_UART
/*****************************************************************************/
/* Define a stream for the printf function, using the UART                   */
/*****************************************************************************/
FILE uart_str = FDEV_SETUP_STREAM(uart_putprintf, NULL, _FDEV_SETUP_WRITE);

/*****************************************************************************/
/* Senden eines C-Strings via printf                                         */
/*****************************************************************************/
int uart_putprintf(char c, FILE *stream)
{
  uart_putc(c);
  return 0;
}
#endif

/*****************************************************************************/
/* Überprüfen ob ein Zeichen Empfangen wurde                                 */
/*****************************************************************************/
uint8_t uart_checkbuffer(void)
{
  uint8_t result = 0;
  if((UART_STATUS_REG & (1<<UART_RECEIVE_COMPLETE))) result = 1;

  return result;
}

/*****************************************************************************/
/* Senden eines Characters                                                   */
/*****************************************************************************/
void uart_putc( char c )
{
  // Warte bis die Sendeeinheit bereit ist
  while(!(UART_STATUS_REG & (1<<UART_DATA_REGISTER_EMPTY)))
    ;
  UART_DATA_REG = c;
}

/*****************************************************************************/
/* Senden eines C-Strings                                                    */
/*****************************************************************************/
void uart_puts(const char* str)
{
  while(*str) {
    uart_putc(*str);
    str++;
  }
}

/*****************************************************************************/
/* Senden eines C-Strings mit fester Länge                                   */
/*****************************************************************************/
void uart_puts_len(const char* str, uint16_t len)
{
  for(uint16_t i = 0; i < len; i++)
  {
    uart_putc(*str);
    str++;
  }
}

/*****************************************************************************/
/* Empfangsbuffer auslesen                                                   */
/*****************************************************************************/
char uart_getc(void)
{
  while(!(UART_STATUS_REG & (1<<UART_RECEIVE_COMPLETE)))
    ;
  return UART_DATA_REG;
}

/*****************************************************************************/
/* String empfangen                                                          */
/*****************************************************************************/
void uart_gets( char* Input )
{
  char c = uart_getc();

  while(c != '\r')
  {
    *Input = c;
    Input++;
    c = uart_getc();
  }
  *Input = '\0';
}

/*****************************************************************************/
/* Initialisierungsroutine. Gewünschte Baudrate wird in uart.h festgelegt    */
/*****************************************************************************/
void uart_init(void)
{
  UART_BAUD_RATE_HIGH	= UBRRH_VALUE;
  UART_BAUD_RATE_LOW = UBRRL_VALUE;
  UART_STATUS_REG = 0x00;
  UART_CONTROL_REG = (1<<UART_ENABLE_TRANSMITTER) | ( 1<<UART_ENABLE_RECEIVER);

#ifdef USE_PRINTF_TO_UART
  stdout = &uart_str;
#endif
}
