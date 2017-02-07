/*
 * uart_put.h
 *
 *  Created on: Nov 1, 2016
 *      Author: skirk
 */

#ifndef UART_PUT_H_
#define UART_PUT_H_
#include <stdint.h>

void uart_putchar(uint8_t tx_data);
void uart_putchar_n(uint8_t * data, uint32_t length);
void uart2_putchar(uint8_t tx_data);

#endif /* UART_PUT_H_ */
