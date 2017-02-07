/*
 * fake_sqr_wave.c
 *
 *  Created on: Dec 7, 2016
 *      Author: skirk
 */

#include "msp.h"
#include "fake_sqr_wave.h"
#include "configuration.h"
#include "waveform_generator.h"
#include "midi.h"
#include <stdio.h>

void generateFakeWave(uint8_t pitch) {
	uint16_t freq = (uint16_t)midiFrequency(pitch);
	uint16_t divider;
	if (freq < 93) {
		TA0CTL &= ~TIMER_A_CTL_ID_MASK;
		TA0CTL |= TIMER_A_CTL_ID_3;  // divide clk by 4 (3 MHz)
		divider = (((SYSCLK/4)/freq)/2);
		if (divider > 50) {
			TA0CCR0 = divider;
			TA0CCTL0 |= CCIE;
		}
	}
	else if (freq < 184) {
		TA0CTL &= ~TIMER_A_CTL_ID_MASK;
		TA0CTL |= TIMER_A_CTL_ID_2; // divide clk by 2 (6 MHz)
		divider = (((SYSCLK/2)/freq)/2);
		if (divider > 50) {
			TA0CCR0 = divider;
			TA0CCTL0 |= CCIE;
		}
	}
	else {
		TA0CTL &= ~TIMER_A_CTL_ID_MASK;  // no clk division (12 MHz)
		divider = (SYSCLK/freq)/2;
		if (divider > 50) {
			TA0CCR0 = divider;  // count up to this value, interrupt will flip pin.
			TA0CCTL0 |= CCIE;   // enable timer interrupts
		}
	}

}
