/*
 * Assignment4.c
 *
 * Created: 2/14/2017 3:02:49 PM
 * Author : Carter W, Drake S
 */ 

// Use sei() and cli() to set and clear interrupt status

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

int checkButtons();
void delay(int ms);
void delay_usec(unsigned int us);

#define OFF_STATE 0
#define CYCLE_STATE 1
#define FLASH_STATE 2

volatile bool button0Pressed = false;
volatile bool button0Released = false;
volatile bool button1Pressed = false;
volatile bool button1Released = false;

int state = OFF_STATE;

int main(void)
{
	//Change input to PORTK0 A8; PORTK1 A9
	DDRK &= ~0x03; //Setting pin 0 and 1 for input
	PORTK |= 0x03; //Setting pin 0 and 1 to idle at high voltage
	PORTF &= ~0x0F; //LEDs off
	
	PCICR |= 0x04;	//Enabling PCIE2 to accept interrupts
	PCMSK2 |= 0x03; //Specifically enabling PCINT16 and 17s
	
	int light_index = 0;
	
	while (1)
	{
		sei();
		//Flashing State
		if(state == FLASH_STATE) {
			//Lights are off
			if(light_index == 0) {
				PORTF |= 0x0F;
				light_index = 1;
				delay(200);
			}
			//Lights are on
			else {
				PORTF &= ~0x0F;
				light_index = 0;
				delay(1000);
			}
		}
		//Cycling State
		else if(state == CYCLE_STATE) {
			PORTF &= ~0x0F;
			light_index = (light_index + 1) % 4;
			PORTF |= (1 << light_index);
			delay(1000);
		}
		//Off State
		else if(state == OFF_STATE) {
			PORTF &= ~0x0F;
			light_index = 0;
			checkButtons();
		}
	}
}

//Break out needed
void delay(int ms) {
	for(int i = 0; i < ms; i++) {
		if(checkButtons())
			break;
		_delay_ms(1);
	}
}

//Returns true if button pressed
int checkButtons() {
	int buttonPress = 0;
	
	//Button 0 has been pressed
	if(button0Released) {
		button0Released = false;
		if(state == CYCLE_STATE)
			state = OFF_STATE;
		else
			state = CYCLE_STATE;
		buttonPress = 1;
	}
	//Button 1 has been pressed
	if(button1Released) {
		button1Released = false;
		if(state == FLASH_STATE)
			state = OFF_STATE;
		else
			state = FLASH_STATE;
		buttonPress = 1;
	}
	
	return buttonPress;
}


ISR(PCINT2_vect)
{
	cli();
	
	// Cycle interrupt
	if(0x01 & PINK)	{ //Checking that PORTK pin0 is high, meaning button has been released
		delay_usec(110);
		if(button0Pressed && (0x01 & PINK)) {
			button0Released = true;
			button0Pressed = false;
		}
	} else //pin is low, button has been pressed
		button0Pressed = true;
		
	// Flashing interrupt
	if(0x02 & PINK)	{ //Checking that PORTK pin0 is high, meaning button has been released
		delay_usec(110);
		if(button1Pressed && (0x02 & PINK)) {
			button1Released = true;
			button1Pressed = false;
		}
	} else //pin is low, button has been pressed
		button1Pressed = true;
		
	sei();
}
