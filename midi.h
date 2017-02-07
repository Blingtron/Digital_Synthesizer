/*
* midi.h
*
*  Created on: Nov 9, 2016
*      Author: skirk
*/

#ifndef MIDI_H_
#define MIDI_H_

#define STATUS_BIT_MASK 0x70 // remember to shift right 4 bits to get value of status
#define CHANNEL_BIT_MASK 0x0F // not used for now

typedef enum channel_status_t {
	NOTE_OFF, // turn a note off
	NOTE_ON, // turn a note on
	AFTERTOUCH, // pressing a key after it's already on (bottoming out)
	CONTROL_CHANGE, // lever/knob value change
	PROGRAM_CHANGE, // patch number change. this specifies the sound used.
	CHANNEL_PRESSURE, // highest pressure key of all keys pressed. we probably dont need it.
	PITCH_BEND, // bend the note
}channel_status;

typedef struct channel_message_t {
	channel_status status; //note on, note off, etc.
	uint8_t channel; // channel number, 0x0 to 0xF (0-15)
	uint8_t pitch; // note
	uint8_t velocity; // how hard the note is hit
	uint8_t size;  // number of bytes in the message
}channel_message;

typedef struct midi_t {
	uint16_t * program; // ptr to wavetable currently in use.
	uint16_t * output; // ptr to current output wave data
}midi;

void initializeMidi(midi * channel, uint16_t * wavetable);
/* 	initializeMidi is called after a midi channel is declared and a wavetable is generated.*/

float midiFrequency(uint8_t pitch);
/* returns the designated frequency of a midi number (pitch) */

uint8_t phaseIncrement(uint8_t pitch);

void noteOn(midi * channel, uint8_t pitch, uint8_t velocity);

void noteOff(midi * channel, uint8_t pitch);

#endif /* MIDI_H_ */
