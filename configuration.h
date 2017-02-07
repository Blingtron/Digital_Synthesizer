/*
 * configuration.h
 *
 *  Created on: Nov 13, 2016
 *      Author: skirk
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#define DAC_SLAVE_ADDR (0x60)
#define SYSCLK (12000000)
#define BAUD_RATE (31250) // standard midi baud rate is 31250

// DMA macros... 1100 0000 0100 0000  0000 0111 1111 0001
#define DST_INC_NONE 0xC0000000
#define DST_BUFFERABLE 0x00600000
#define ARB_SIZE_1024 0x00028000
#define XFER_SIZE 0x000007F0  // n-1 transfers  07F = 128 - 1
#define XFER_AUTO_REQ 0x00000002
#define BASIC_MODE 0x00000001

void configure_pins(void);

void configure_UART(void);

void configure_I2C(void);

void i2c_transmit(void);

void configure_timer(void);

void configure_DMA(uint32_t * ctlbase, uint32_t src_end_address, uint32_t dst_end_address);
/*
 * As of now ctlbase should be preallocated with 12 bytes to store DMA configuration. (I think...
 *   I gave it more just in case and to easily see in the memory browser)
 * ctlbase allocation could probably be put into this function.
 */

void configure_clocks(void);

#endif /* CONFIGURATION_H_ */
