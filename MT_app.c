/**
* @file
* @brief Testumgebung für Multithreading-Bibliothek
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/power.h>
#include "MT.h"

#define INT0_OFF EIMSK &= ~(1<<0)
#define INT0_ON  EIMSK |= (1<<0)

/*
	PORTD == BUTTONS !!!
	PORTB == LEDS !!!
*/
void lauflicht();
void toggle6();


uint8_t stack1[MT_STACKSIZE]; /**< @brief Speicher für Stack 1 */
uint8_t stack2[MT_STACKSIZE]; /**< @brief Speicher für Stack 2 */
uint8_t stack3[MT_STACKSIZE]; /**< @brief Speicher für Stack 3 */


void pwm_thread()
{

	TCCR0A = (1<<7)|(1<<1)|(1<<0);/* WGM02..WGM00 = 011b -> Fast PWM, TOP=0xFF, 
									Update of OCRx at TOP, TOV flag set on 0xFF*,
									COM0A1..COM0A0 = 10b -> Clear OC0A on Compare Match*/
	TCCR0B = (1<<0);	/* WGM02=0 (s. oben), CS02..CS00=001b -> clkI/O/(No prescaling) */
	
	
	/*The Output Compare Register A contains an 8-bit value that is continuously compared with the
	  counter value (TCNT0). A match can be used to generate an Output Compare interrupt, or to
	  generate a waveform output on the OC0A pin.*/
	OCR0A = 0xFE;
	TCNT0=0;


	while(1)
	{
		for(uint8_t dc=0x50;dc<0xFF;dc++)
		{

			OCR0A=dc;
			_delay_ms(1);

		}
	
		_delay_ms(100);

		for(uint8_t dc=0xFF;dc>0x50;dc--)
		{

			OCR0A=dc;
			_delay_ms(1);

		}

	}
}




void lauflicht()
//erzeugt Lauflicht von LED1-6 
{
	uint16_t i;
	uint8_t bit=0;
	while(1)
	{
		for(i=0;i<12000;i++) asm volatile("nop");
		
		cli();
		PORTB ^= (1<< (bit+1));
		sei();

		bit=(bit+1)%5;
	}
}


void do_toggle6()
{

	cli();
	PORTB ^= (1<<6);
	sei();


}


void toggle6()
{

	uint16_t i,j;
	uint32_t k;
	//for(k=0;k<50;k++)
	while(1)
	{
		for(i=0;i<65000;i++) 
		{
			j=j+2;
			j=j-2;
		}

		do_toggle6();

	}
}





void HW_init()
{
	
	clock_prescale_set(clock_div_256);/*16MHz/256=62,5kHz*/


	//PD0 == INT0 == Pin 43
	DDRD=0;
	DDRB=0xFF;//LED's
	PORTB=0xFF;

	EICRA |= (1<<0)|(1<<1);//rising edge INT0
	//EIMSK |= (1<<0);
	
}


/**
* @brief Startpunkt der Testumgebung
* 
*/
int main()
{
	uint16_t delay;	
	cli();

	HW_init();
	MT_init();

	MT_TCB tcb1,tcb2,tcb3;

	MT_start_thread(&tcb1,(PGM_VOID_P)(&lauflicht),stack1,	MT_STACKSIZE);
	//MT_start_thread(&tcb2,(PGM_VOID_P)(&toggle6),stack2,	MT_STACKSIZE);
	MT_start_thread(&tcb3,(PGM_VOID_P)(&pwm_thread),stack3,	MT_STACKSIZE);


	sei();
	while(1)
	{
		for(uint16_t i=0;i<500;i++) 
		{
			delay=delay+2;
			delay=delay-2;

		}
		cli();
		PORTB ^= (1<<0);		
		sei();
	}
	
}
