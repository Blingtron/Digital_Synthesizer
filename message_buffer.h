/*
 * message_buffer.h
 *
 *  Created on: Nov 21, 2016
 *      Author: skirk
 */

#ifndef MESSAGE_BUFFER_H_
#define MESSAGE_BUFFER_H_

typedef struct message_buffer_t{
	uint8_t * buffer;
	uint8_t * first;
	uint8_t * last;
	uint8_t length;
	uint8_t max_size;
}message_buffer;

typedef enum buffer_test_t{
	SUCCESS,
	BUFFER_FULL,
	BUFFER_EMPTY,
	ALLOCATION_ERROR
}buffer_test;

buffer_test initializeBuffer(message_buffer * buf, uint8_t max_size);

buffer_test addItemToBuffer(message_buffer * buf, uint8_t data);

buffer_test getItemFromBuffer(message_buffer * buf, uint8_t * container);
/* removes the oldest item in the buffer, places in container.*/

void clearBuffer(message_buffer * buf);

uint8_t isEmpty(message_buffer * buf);

uint8_t isFull(message_buffer * buf);

#endif /* MESSAGE_BUFFER_H_ */
