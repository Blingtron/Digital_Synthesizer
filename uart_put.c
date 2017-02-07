/*
 * uart_put.c
 *
 *  Created on: Nov 1, 2016
 *      Author: skirk
 */
#include "uart_put.h"
#include "msp.h"

void uart_putchar_n(uint8_t * data, uint32_t length) {
	volatile uint32_t i;
	for(i = 0; i < length; i++) {
		uart_putchar(*data++);
	}
}

void uart_putchar(uint8_t tx_data) {
	UCA0TXBUF = tx_data;     // load data onto buffer.
}

void uart2_putchar(uint8_t tx_data) {
		//eusciA2 putchar function.
	UCA2TXBUF = tx_data;     // load data onto buffer.
}
