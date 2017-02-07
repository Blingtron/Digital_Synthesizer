/*
 * configuration.c
 *
 *  Created on: Nov 13, 2016
 *      Author: skirk
 */
#include "msp.h"
#include "configuration.h"
#include <stdlib.h>

void configure_pins(void) {
	P1DIR |= BIT0;       // 1.0: red led (note on indication)
	P1OUT &= ~BIT0;
	P6DIR |= BIT5|BIT4;  // 6.4: wave generation pin (speaker)
	P6OUT &= ~BIT5|BIT4; // 6.5: wave generation pin (speaker)

	//i2c (euscib0)
	P1SEL0 |= (BIT6 | BIT7);
	P1SEL1 &= ~(BIT6 | BIT7);
	P1DIR &= ~(BIT6 | BIT7);
	P1REN |= (BIT6 | BIT7);
	P1OUT |= (BIT6 | BIT7);

	// uart (eusciA0 and eusciA2)
	P1SEL1 &= ~(BIT2|BIT3);
	P1SEL0 |= BIT2|BIT3;
	P3SEL1 &= ~(BIT2|BIT3);
	P3SEL0 |= BIT2|BIT3;

	// on-board buttons
	P1DIR &= ~(BIT1|BIT4);
	P1OUT |= BIT1|BIT4;
	P1REN |= BIT1|BIT4;
	P1IFG &= ~(BIT1|BIT4);
	P1IES |= BIT1;
	P1IE |= BIT1|BIT4;
	NVIC_EnableIRQ(PORT1_IRQn);

	// big red button
	P2DIR &= ~BIT7;
	P2OUT |= BIT7;
	P2REN |= BIT7;
	P2IFG &= BIT7;
	P2IES |= BIT1;
	P2IE |= BIT7;
	NVIC_EnableIRQ(PORT2_IRQn);
}

void configure_UART(void) {
	UCA0CTLW0 |= UCSWRST;     // Put eUSCI in reset mode
	UCA0CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
	UCA0BR0 = 0x80;  // baud rate, should be parameterized
	UCA0BR1 = 0x01;          // overflow
	UCA0CTLW0 &= ~UCSWRST;    // Initialize eUSCI
	UCA0IE &= ~UCTXIE;
	UCA0IE |= UCRXIE;     // enable USCI_A0 RX interrupts.
	NVIC_EnableIRQ(EUSCIA0_IRQn);

	UCA2CTLW0 |= UCSWRST;
	UCA2CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
	UCA2BR0 = 0x80;
	UCA2BR1 = 0x01;
	UCA2CTLW0 &= ~UCSWRST;
	UCA2IE &= ~UCTXIE;
	UCA2IE |= UCRXIE;
	NVIC_EnableIRQ(EUSCIA2_IRQn);
}

void configure_DMA(uint32_t * ctlbase, uint32_t src_end_address, uint32_t dst_end_address) {
	DMA_Control->CTLBASE = (uint32_t)ctlbase;  // CTLBASE address
	*(uint32_t *)(DMA_Control->CTLBASE + 0x08) = DST_INC_NONE | XFER_SIZE | BASIC_MODE | DST_BUFFERABLE;
	*(uint32_t *)(DMA_Control->CTLBASE + 0x04) = dst_end_address;
	*(uint32_t *)(DMA_Control->CTLBASE) = src_end_address;
	DMA_Channel->CH_SRCCFG[0] = 1; // source: euscia0 TX interrupt flag
	DMA_Channel->INT1_SRCCFG = DMA_INT1_SRCCFG_EN; // channel 0 finished interrupt
	DMA_Control->ENASET |= BIT0; // enables channel 0 of DMA
	DMA_Control->CFG |= BIT0; // enable DMA controller
	NVIC_EnableIRQ(DMA_ERR_IRQn);
	NVIC_EnableIRQ(DMA_INT1_IRQn);
}

void configure_I2C(void) {
	UCB0CTLW0 = UCSWRST;                         // Enable SW reset
	UCB0CTLW0 |= EUSCI_B_CTLW0_MODE_3 | UCSSEL_2 | UCMST | UCSYNC;
			// I2C mode, smclk, I2C Master, transmitter, synchronous mode
	UCB0CTLW1 = EUSCI_B_CTLW1_ASTP_2;				// auto-stop enable
    UCB0TBCNT = 0x0002; 							// number of bytes to be received
    UCB0BR0 = 0x14;									// ~100khz
    UCB0BR1 = 0x00;
    UCB0CTLW0 &= ~UCSWRST;                       // Clear SW reset, resume operation
    UCB0I2CSA |= DAC_SLAVE_ADDR;					// set slave address
    UCB0CTLW0 |= UCTXSTT | UCTR;
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIE;     //Enable RX and TX interrupt
	NVIC_EnableIRQ(EUSCIB0_IRQn);
}

void configure_timer(void) {
	// timer is reconfigured at run time by note on/off messages.
	TA0CTL |= TASSEL_2 | TIMER_A_CTL_MC__UP;  // SMCLK, up-down mode
	NVIC_EnableIRQ(TA0_0_IRQn);
}

void i2c_transmit(void){
	//while (UCB0CTLW0 & UCTXSTP); 		// Ensure stop condition got sent
	//UCB0CTLW0 &= ~UCSLA10;				// Select size of address
	while(UCB0STAT & UCBBUSY);
	//UCB0CTLW0 |= UCTXSTT;			// Transmit mode and generate start
	UCB0TXBUF = 0xAA;
	while(!(UCB0IFG & UCTXIFG));
	UCB0TXBUF = 0xFF;
	UCB0CTLW0 |= UCTXSTP;				// Stop condition
}

void configure_clocks(void) {
	CS->KEY = 0x695A;           // unlock CS module for register access
	CS->CTL0 = 0;               // reset tuning parameters
	CS->CTL0 = CS_CTL0_DCORSEL_3; // set DC0 to 12 MHz (middle of 8-16 range)
	// Select ACLK = REPO, SMCLK = MCLK = DCO
	CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
	CS->KEY = 0;                 // lock CS module for register access
}


