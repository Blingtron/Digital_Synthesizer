/*
* waveform_generator.c
*
*  Created on: Nov 11, 2016
*      Author: skirk
*/
#include "waveform_generator.h"
#include <msp.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define PI (3.14159265359)

uint16_t * squareWave() {
	uint16_t * sqrWave = (uint16_t *)calloc(WAVE_SAMPLES, 2);
	if (!sqrWave) {
		return 0;  // logging or pin flip for allocation error
	}
	uint16_t i;
	for (i = 0; i < WAVE_SAMPLES; i++) {
		if (i > WAVE_SAMPLES / 2) {  // half-way through period, symmetrical square.
			sqrWave[i] = MAX_N_DAC;
		}
		else {
			sqrWave[i] = 0;
		}
	}
	return sqrWave;
}

uint16_t * sineWave() {
	uint16_t * sinWave = (uint16_t *)calloc(WAVE_SAMPLES, 2);
	uint16_t i;
	float phase = 0;
	for (i = 0; i < WAVE_SAMPLES; i++) {
		sinWave[i] = (MAX_N_DAC/2) + ((MAX_N_DAC/2) * sin(phase));
		phase = phase + ((2.0 * (float)PI) / (float)WAVE_SAMPLES); // increment by the phase resolution
	}  // phase should never exceed 2*pi or else it won't wrap around.
	return sinWave;
}

uint16_t * sawWave() {
	return 0; // implement later
}

uint16_t * triangleWave() {
	return 0;  // implement later
}
