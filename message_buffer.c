/*
 * message_buffer.c
 *
 *  Created on: Nov 21, 2016
 *      Author: skirk
 */

#include "msp.h"
#include "message_buffer.h"
#include <stdlib.h>

buffer_test initializeBuffer(message_buffer * buf, uint8_t max_size) {
	buf->buffer = (uint8_t *)malloc(max_size);
	if(!buf->buffer) {
		return ALLOCATION_ERROR;
	}
	buf->first = buf->buffer;
	buf->last = buf->buffer;
	buf->max_size = max_size;
	buf->length = 0;
	return SUCCESS;
}

void clearBuffer(message_buffer * buf) {
	buf->first = buf->buffer;
	buf->last = buf->buffer;
	buf->length = 0;
}

uint8_t isEmpty(message_buffer * buf){
	if(buf->length == 0) {
		return 1;
	}
	else return 0;
}

uint8_t isFull(message_buffer * buf){
	if(buf->length == buf->max_size) {
		return 1;
	}
	else return 0;
}

buffer_test addItemToBuffer(message_buffer * buf, uint8_t data) {
	if(isFull(buf)) {
		return BUFFER_FULL;
	}
	buf->length++;
	*buf->first = data;
	buf->first++;
	return SUCCESS;
}

buffer_test getItemFromBuffer(message_buffer * buf, uint8_t * container) {
	if(isEmpty(buf)) {
		return BUFFER_EMPTY;
	}
	*container = *buf->last;
	buf->last++;
	if(buf->last > (buf->first)) {
		buf->last = buf->buffer; // edge
		buf->first = buf->buffer;
		buf->length = 0;
		return SUCCESS;
	}
	buf->length--;
	return SUCCESS;
}



