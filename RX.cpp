// 
// 
// 

#include "RX.h"
uint8_t RX::rcChannel[] = { ROLLPIN, PITCHPIN, YAWPIN, THROTTLEPIN, AUX1PIN, AUX2PIN, AUX3PIN, AUX4PIN };
uint8_t RX::PCInt_RX_Pins[] = { PCINT_RX_BITS };
volatile uint16_t RX::rcValue[] = { 1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502 };
uint8_t RX::rc4ValuesIndex;
uint16_t RX::rcData4Values[RC_CHANS][4];
uint16_t RX::rcDataMean[RC_CHANS];
int16_t RX::rcData[RC_CHANS];
void RX::configureReceiver() { //Put this in setup()
	/******************    Configure each rc pin for PCINT    ***************************/
	DDRK = 0;  // defined PORTK as a digital input port ([A8-A15] are consired as digital PINs and not analogical)
	// PCINT activation
	for (uint8_t i = 0; i < PCINT_PIN_COUNT; i++){ // i think a for loop is ok for the init.
		PCINT_RX_PORT |= PCInt_RX_Pins[i]; //Enable pull-up resistors for required pins
		PCINT_RX_MASK |= PCInt_RX_Pins[i]; //Enable change interrupt at PORTK pins
	}
	PCICR = PCIR_PORT_BIT; //Enable PORTK pin change interrupt (PCMSK2 enable)
	sei();
}


// predefined PC pin block (thanks to lianj)  - Version without failsafe
uint16_t RX::readRawRC(uint8_t chan) {
	uint16_t data;
	uint8_t oldSREG;
	oldSREG = SREG; cli(); // Let's disable interrupts
	data = rcValue[rcChannel[chan]]; // Let's copy the data Atomically
	SREG = oldSREG;        // Let's restore interrupt state
	return data; // We return the value correctly copied when the IRQ's where disabled
}

void RX::computeRC() { //Add this to loop()
	uint8_t chan, a;
	rc4ValuesIndex++;
	if (rc4ValuesIndex == 4) rc4ValuesIndex = 0;
	for (chan = 0; chan < RC_CHANS; chan++) {
		rcData4Values[chan][rc4ValuesIndex] = readRawRC(chan);
		rcDataMean[chan] = 0;
		for (a = 0; a<4; a++) rcDataMean[chan] += rcData4Values[chan][a];
		rcDataMean[chan] = (rcDataMean[chan] + 2) >> 2;
		if (rcDataMean[chan] < (uint16_t)rcData[chan] - 3)  rcData[chan] = rcDataMean[chan] + 2;
		if (rcDataMean[chan] > (uint16_t)rcData[chan] + 3)  rcData[chan] = rcDataMean[chan] - 2;
	}
}