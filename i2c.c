#include "msp.h"
#include "i2c.h"

void sendToI2C(uint8_t * data) {
	while (!(UCB0IFG & EUSCI_B_IFG_TXIFG));           // Block until transmitter is ready.
		UCB0TXBUF = data;     // load data onto buffer.
}
