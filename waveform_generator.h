/*
* waveform_generator.h
*
*  Created on: Nov 11, 2016
*      Author: skirk
*/

#ifndef WAVEFORM_GENERATOR_H_
#define WAVEFORM_GENERATOR_H_

#define WAVE_SAMPLES 128 // max samples allocated for each waveform:
				//** when this is changed, DMA XFER COUNT must be updated as well! **//
#define SAMPLE_RATE 5000 // DAC sample rate
#define MAX_N_DAC 0x0FFF  // max DAC integer

/*  WAVE FUNCTIONS:
 * 		-> always return a pointer to an allocated array of 16bit wave values on the heap (wavetable)
 *      -> pointer should be passed into midiInitialization() or program change function to be assigned to a midi channel
 *      -> generated wavetables remain allocated for reuse, but can be deallocated using free().
 */
uint16_t * sineWave();
uint16_t * squareWave();
uint16_t * triangleWave();
uint16_t * sawWave();

#endif /* WAVEFORM_GENERATOR_H_ */
