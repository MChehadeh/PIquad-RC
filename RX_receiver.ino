#include <avr/wdt.h>
#include "os.h"
#include "communication.h"
#include "profiler.h"
#include "RX.h"
#define main_serial Serial

#define RX_PIN_CHECK(pin_pos, rc_value_pos)    if (mask & RX::PCInt_RX_Pins[pin_pos]) {  if (!(pin & RX::PCInt_RX_Pins[pin_pos])) { dTime = cTime-edgeTime[pin_pos];   if (900<dTime && dTime<2200) {        RX::rcValue[rc_value_pos] = dTime;}    } else edgeTime[pin_pos] = cTime;  }

ISR(RX_PC_INTERRUPT) { //this ISR is common to every receiver channel, it is call everytime a change state occurs on a RX input pin
	uint8_t mask;
	uint8_t pin;
	uint16_t cTime, dTime;
	static uint16_t edgeTime[8];
	static uint8_t PCintLast;
	pin = RX_PCINT_PIN_PORT; // RX_PCINT_PIN_PORT indicates the state of each PIN for the arduino port dealing with Ports digital pins

	mask = pin ^ PCintLast;   // doing a ^ between the current interruption and the last one indicates wich pin changed
	cTime = micros();         // micros() return a uint32_t, but it is not usefull to keep the whole bits => we keep only 16 bits
	sei();                    // re enable other interrupts at this point, the rest of this interrupt is not so time critical and can be interrupted safely
	PCintLast = pin;          // we memorize the current state of all PINs [D0-D7]
	RX_PIN_CHECK(0, 2);
	RX_PIN_CHECK(1, 4);
	RX_PIN_CHECK(2, 5);
	RX_PIN_CHECK(3, 6);
	RX_PIN_CHECK(4, 7);
	RX_PIN_CHECK(5, 0);
	RX_PIN_CHECK(6, 1);
	RX_PIN_CHECK(7, 3);
}
os main_os;
void setup()
{
	MCUSR = 0;
	wdt_disable();
	interrupts();
	main_serial.begin(115200);
	delay(100);
	//main_os.boot();
	/* add setup code here */
	main_os.boot();
}

void loop()
{

	while (1){
		main_os.perform_loop();
	}
	/* add main program code here */

}
