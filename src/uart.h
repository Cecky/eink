/******************************************************************************
* uart.h : UART handling                                                      *
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
#ifndef UART_H
#define UART_H

#define BAUD  38400
#define USE_PRINTF_TO_UART

#include <avr/io.h>
#include <stdio.h>
#include <util/setbaud.h>

/*
 *  Defines for the various USART registers
 */
#if  defined(__AVR_ATmega8__)    || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__) 
/* 
 * ATMega with one USART
 */ 
	#define	UART_BAUD_RATE_LOW          UBRRL
	#define UART_BAUD_RATE_HIGH         UBRRH
	#define	UART_STATUS_REG             UCSRA
	#define	UART_CONTROL_REG            UCSRB
	#define	UART_ENABLE_TRANSMITTER     TXEN
	#define	UART_ENABLE_RECEIVER        RXEN
	#define	UART_TRANSMIT_COMPLETE      TXC
	#define	UART_RECEIVE_COMPLETE       RXC
	#define	UART_DATA_REG               UDR
	#define UART_DOUBLE_SPEED           U2X
	#define UART_DATA_REGISTER_EMPTY    UDRE

#elif  defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega162__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) \
	|| defined(__AVR_AT90CAN32__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN128__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) \
  || defined(__AVR_ATmega328PA__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega324A__)
/* 
 *  ATMega with two USART
 */ 
	#define	UART_BAUD_RATE_LOW          UBRR0L
	#define UART_BAUD_RATE_HIGH         UBRR0H
	#define	UART_STATUS_REG             UCSR0A
	#define	UART_CONTROL_REG            UCSR0B
	#define	UART_ENABLE_TRANSMITTER     TXEN0
	#define	UART_ENABLE_RECEIVER        RXEN0
	#define	UART_TRANSMIT_COMPLETE      TXC0
	#define	UART_RECEIVE_COMPLETE       RXC0
	#define	UART_DATA_REG               UDR0
	#define UART_DOUBLE_SPEED           U2X0
	#define UART_DATA_REGISTER_EMPTY    UDRE0
#else
	#error "no UART definition for MCU available"
#endif

#ifdef USE_PRINTF_TO_UART
  int uart_putprintf(char c, FILE *stream);
#endif
uint8_t uart_checkbuffer(void);
void uart_putc( char c );
void uart_puts( const char* str );
void uart_puts_len(const char* str, uint16_t len);
char uart_getc(void);
void uart_gets( char* Input );
void uart_init(void);


#endif
