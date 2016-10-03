// RX.h

#ifndef _RX_h
#define _RX_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
//#include <avr/interrupt.h>
//#  define ISR(vector, ...)            \
//    extern "C" void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
//    void vector (void)
//extern "C" void vector(void) __attribute__((signal, __INTR_ATTRS));
//RX PIN assignment inside the port //for PORTK
#define THROTTLEPIN                0  //PIN 62 =  PIN A8
#define ROLLPIN                    1  //PIN 63 =  PIN A9
#define PITCHPIN                   2  //PIN 64 =  PIN A10
#define YAWPIN                     3  //PIN 65 =  PIN A11
#define AUX1PIN                    4  //PIN 66 =  PIN A12
#define AUX2PIN                    5  //PIN 67 =  PIN A13
#define AUX3PIN                    6  //PIN 68 =  PIN A14
#define AUX4PIN                    7  //PIN 69 =  PIN A15
#define V_BATPIN                   A0    // Analog PIN 0
#define PSENSORPIN                 A2    // Analog PIN 2
#define PCINT_PIN_COUNT            8
#define PCINT_RX_BITS              (1<<2),(1<<4),(1<<5),(1<<6),(1<<7),(1<<0),(1<<1),(1<<3)
#define PCINT_RX_PORT              PORTK
#define PCINT_RX_MASK              PCMSK2 //Atmega2560 register for PORTK change interrupt
#define PCIR_PORT_BIT              (1<<2)
#define RX_PC_INTERRUPT            PCINT2_vect
#define RX_PCINT_PIN_PORT          PINK //Writing to PINK toggles PORTK. Reading PINK is a safe read of PORTK
#define RC_CHANS 8						//Normal receiver
//////////////////////////////////////////////////////

class RX
{
private:


public:
	static int16_t rcData[RC_CHANS];    // interval [1000;2000], this will be the final result


	//RAW RC values will be store here
	static volatile uint16_t rcValue[RC_CHANS]; // interval [1000;2000]
	// Standard Channel order
	static uint8_t rc4ValuesIndex;
	static uint16_t rcData4Values[RC_CHANS][4], rcDataMean[RC_CHANS];
	static uint8_t rcChannel[RC_CHANS];
	static uint8_t PCInt_RX_Pins[PCINT_PIN_COUNT]; // if this slowes the PCINT readings we can switch to a define for each pcint bit
	/**************************************************************************************/
	/***************                   RX Pin Setup                    ********************/
	/**************************************************************************************/
	void static configureReceiver();
	// predefined PC pin block (thanks to lianj)  - Version without failsafe
	static uint16_t readRawRC(uint8_t chan);

	static void computeRC();


};




#endif

