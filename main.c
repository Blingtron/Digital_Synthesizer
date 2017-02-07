//*****************************************************************************
//
// MSP432 DIGITAL SYNTHESIZER - main.c
//
//****************************************************************************

/*****************************NOTES**********************************************
 *
 * to test with realterm over uart:
 * - realterm should be repeating "FF 07" start, this is middle voltage (flat-line)
 * 	-> Send a NOTE ON message with realterm ("0x90 0x30 0x7f" should turn on note 0x30 at highest velocity)
 * 	   using "send numbers".
 * 	-> NOTE OFF message turns off the output ("0x80 0x30 0x7f" for example) and resets
 * 	   to middle voltage flat-line. Sometimes it gets stuck (on NOTE OFF especially, need to debug this) so you have
 * 	   to send the message again and it usually figures it out.
 * 	-> DMA is setup to place data into eUSCI_A0 TX buffer right now, so timing is very slow. it can be changed to the I2C TX buffer
 * 	   when that is all setup and it will be fast.
 *
 * 	        -sk
 *
 * 	12/7 Notes:
 * 	-> I2C never worked, so the DMA and the wavetable synthesis portions of the code serve no purpose.
 * 	-> some of the wavetable code has been commented out since it doesn't do anything but waste resources at this point.
 * 	-> DMA functionality code has been commented out in-line in all executable code, functions have been left intact.
 *	-> Errors could be caused by the "bottoming out" functionality of the controller, or other messages we didn't account for,
 *	 	we need to find the value of those messages and throw them out instead of placing in buffer.
 *
 ********************************************************************************
 */

#include "msp.h"
#include <stdlib.h>
#include "configuration.h"
#include "waveform_generator.h"
#include "midi.h"
#include "uart_put.h"
#include "message_buffer.h"
#include "fake_sqr_wave.h"

channel_message current_message;
message_buffer message_buf;
midi channel1;
uint8_t two_byte_flag = 0;


void main(void) {
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    configure_clocks();
    configure_pins();
    configure_UART();
    configure_I2C();
    configure_timer();

    initializeBuffer(&message_buf, 3); // initalize buffer for 3 byte messages
    uint8_t temp_buff;
    uint32_t * dma_ctlbase = (uint32_t *)malloc(32);
    uint16_t * sqrWave = squareWave();  // used only for wavetable synthesis/DMA, unused now.
    initializeMidi(&channel1, sqrWave);
    configure_DMA(dma_ctlbase, ((uint32_t)channel1.output + (WAVE_SAMPLES * 2) - 1), 0x4000100E);
    __enable_interrupt();

    while(1) {
    	while(!isFull(&message_buf));     // wait for 3 byte message to be gathered.

    	// get the bytes from buffer, place in current message in expected order.
    	getItemFromBuffer(&message_buf, &temp_buff);
    	current_message.status = (channel_status)((temp_buff & STATUS_BIT_MASK) >> 4);
    	getItemFromBuffer(&message_buf, &current_message.pitch);
    	getItemFromBuffer(&message_buf, &current_message.velocity);

    	// do things based on the status of the message.
    	if (current_message.status == NOTE_ON) {
    		noteOn(&channel1, current_message.pitch, current_message.velocity);
    		clearBuffer(&message_buf);

    		// DMA needs to be re-enabled every time the output is changed, not completely sure why yet.
    		*(uint32_t *)(DMA_Control->CTLBASE + 0x08) = DST_INC_NONE | XFER_SIZE | BASIC_MODE | DST_BUFFERABLE;
    		DMA_Control->ENASET |= BIT0;
    		P1OUT |= BIT0; // turn led on, note should be playing.
    	}
    	if (current_message.status == NOTE_OFF) {
    		noteOff(&channel1, current_message.pitch);
    		clearBuffer(&message_buf);
    		*(uint32_t *)(DMA_Control->CTLBASE + 0x08) = DST_INC_NONE | XFER_SIZE | BASIC_MODE | DST_BUFFERABLE;
    		DMA_Control->ENASET |= BIT0;
    		P1OUT &= ~BIT0; // turn led off, no note should be playing.
    	}
    }
}

void EUSCIA0_IRQHandler(void) {
	/*
	 * EUSCIA0 UART is used for logging, and if desired sending midi messages.
	 * controller will accept these messages as if they were received from
	 * a midi controller.
	 */
	uint8_t rxdata;
	if(UCA0IFG & UCRXIFG) {
		UCA0IFG &= ~UCRXIFG;
		if (!two_byte_flag) {
			uart_putchar(UCA0RXBUF); // log function here.
			addItemToBuffer(&message_buf, UCA0RXBUF);
		}
		else if (two_byte_flag == 1) {
			two_byte_flag = 0;
			clearBuffer(&message_buf);
		}
		if (rxdata == 0xD0) {
			two_byte_flag++;
		}
	}
}

void EUSCIA2_IRQHandler(void) {
	 /* EUSCIA2 UART comms with the midi controller (31250 BAUD)
	  * The RX interrupt logs received messages over EUSCIA0 and
	  * places the received data into the message buffer.
	  * two_byte_flag ensures 2 byte messages do not cause sync
	  * issues with the message buffer. */
	uint8_t rxdata;
	if(UCA2IFG & UCRXIFG) {
		UCA2IFG &= ~UCRXIFG;
		if (!two_byte_flag) {
			uart_putchar(UCA2RXBUF); // log function here.
			addItemToBuffer(&message_buf, UCA2RXBUF);
		}
		else if (two_byte_flag == 1) {
			two_byte_flag = 0;
			clearBuffer(&message_buf);
		}
		if (rxdata == 0xD0) {
			two_byte_flag++;
		}
	}
}

void EUSCIB0_IRQHandler(void) {
	// DAC -> I2C interrupts
	if(UCB0IFG & UCRXIFG) {
		__no_operation();
	}
	if(UCB0IFG & UCTXIFG) {
		__no_operation();
	}
}

void DMA_INT1_IRQHandler(void) {
	// this interrupt is called when the DMA completes a cycle through the whole waveform. I think there's a way
	// to make it repeat cycles without interrupt, but I haven't figured it out yet.
	*(uint32_t *)(DMA_Control->CTLBASE + 0x08) = DST_INC_NONE | XFER_SIZE | BASIC_MODE | DST_BUFFERABLE;
	DMA_Control->ENASET |= BIT0;
}

void TA0_0_IRQHandler(void) {
	// flip a pin for square wave generation
	TA0CCTL0 &= ~CCIFG;
	P6OUT ^= BIT5|BIT4;
}

void DMA_ERR_IRQHandler(void) {
	// called when DMA controller hits an error. should perform logging.
	__no_operation();
}

void PORT2_IRQHandler(void) {
	// big red button emergency reset:
	// this clears the message buffer, turns off all output, and sets the
	// timer CCR value arbitrarily high so that it may exit a loop
	uint16_t i;
	if (P2IFG & BIT7) {
		P2IFG &= ~(BIT7);
		for(i = 0; i < 2000; i++);
		clearBuffer(&message_buf);
		noteOff(&channel1, 1);
		P1OUT &= ~BIT0;
		TA0CCR0 |= 9000;
	}
}
