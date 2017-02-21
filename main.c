/*
 * Assignment4.c
 *
 * Created: 2/14/2017 3:02:49 PM
 * Author : Carter W, Drake S
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

int checkButtons();
void delay(int ms);

#define OFF_STATE 0
#define CYCLE_STATE 1
#define FLASH_STATE 2

volatile bool button0Pressed = false;
volatile bool button1Pressed = false;
int state = OFF_STATE;

int curr6 = 1;
int prev6 = 1;

int curr7 = 1;
int prev7 = 1; 

int main(void)
{
	//Change input to PORTK0 A8; PORTK1 A9
	DDRF &= ~0xC0; //Setting pin 6 and 7 for input
	PORTF |= 0xC0; //Setting pin 6 and 7 to idle at high voltage
	PORTF &= ~0x0F; //LEDs off
	
	int light_index = 0;
	
    while (1)
    {
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
	if(curr6 && !prev6) {
		if(state == CYCLE_STATE)
			state = OFF_STATE;
		else
			state = CYCLE_STATE;
		buttonPress = 1;
	}
	//Button 1 has been pressed
	if(curr7 && !prev7) {
		if(state == FLASH_STATE)
			state = OFF_STATE;
		else
			state = FLASH_STATE;
		buttonPress = 1;
	}
	
	prev6 = curr6;
	curr6 = 0x40 & PINF; //0100 0000
	
	prev7 = curr7;
	curr7 = 0x80 & PINF; //1000 0000
	
	return buttonPress;
}

