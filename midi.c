/*
* midi.c
*
*  Created on: Nov 9, 2016
*      Author: skirk
*/

#include "msp.h"
#include <stdint.h>
#include "waveform_generator.h"
#include "midi.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fake_sqr_wave.h"

void initializeMidi(midi * channel, uint16_t * wavetable) {
	uint32_t i;
	channel->program = (uint16_t *)calloc(WAVE_SAMPLES, 2);
	channel->output = (uint16_t *)calloc(WAVE_SAMPLES, 2);
	memmove(channel->program, (void*)wavetable, WAVE_SAMPLES * 2);
	for(i = 0; i < WAVE_SAMPLES; i++){
		*(channel->output + i) = MAX_N_DAC / 2;
	}
}

float midiFrequency(uint8_t pitch) {
	return pow(2.0, (((float)pitch - 69.0) / 12.0)) * 440.0;
}

uint8_t phaseIncrement(uint8_t pitch) {
	return (WAVE_SAMPLES * midiFrequency(pitch))/SAMPLE_RATE;
	// truncated for now. this can get complicated the more precise we want it.
}

void noteOn(midi * channel, uint8_t pitch, uint8_t velocity) {
	generateFakeWave(pitch); // work-around for I2C not working.

	/*************/
	//uint8_t i;
	//uint8_t p = phaseIncrement(pitch);
	//for (i = 0; i < WAVE_SAMPLES; i++) {
		// mod wave_samples so if pointer exceeds length of wavetable it wraps around.
	//	*(channel->output + i) = channel->program[((p*i) % WAVE_SAMPLES)];
	//}
	/*************/
}

void noteOff(midi * channel, uint8_t pitch) {
	TA0CCTL0 &= ~CCIE; // disable timer interrupts
	P1OUT &= ~BIT5;    // ensure output off.
	/*************/
	//uint8_t i;
	//for (i = 0; i < WAVE_SAMPLES; i++) {
	//	*(channel->output + i) = MAX_N_DAC / 2; // flat middle voltage
	//}
	/*************/
}
