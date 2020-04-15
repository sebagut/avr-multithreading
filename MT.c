/**
* @file
* @brief Multithreading-Bibliothek Hauptmodul
*
*/

#include <avr/interrupt.h>
#include "MT.h"



static MT_TCB main_tcb;
static MT_TCB *current_thread;



volatile uint8_t _SPH, _SPL, SR;


/**
* @brief Initialisierung der Multithreading-Funktionalität.
*
* @detail Initialisiert die Thread-Liste mit dem Thread-Control-Block für die 
*         Main-Funktion und startet den Timer 1
* @param void
* @return void
*/
void MT_init(void)
{
	main_tcb.next= &main_tcb;
	current_thread=&main_tcb;

	
	//Timer 1 (16Bit)
	TCCR1A=0;
	//TCCR1B = (1<<3)|(1<<2)|(1<<0); //CTC an, clk/1024
	TCCR1B = (1<<3)|(1<<1); //CTC an, clk/8
	OCR1A  =  200; //0x3D09; //1 Sekunde bei 16 MHz und 1/1024, 62,5kHz/8/200->39 mal pro Sekunde 
	TIMSK1 = (1<<1);


}


/**
* @brief Schaltet zum nächsten Thread in der Liste um.
* @param void
* @return void
*/
void schedule(void)
{
	current_thread->sph=_SPH;
	current_thread->spl=_SPL;
	current_thread = current_thread->next;
	_SPH=current_thread->sph;
	_SPL=current_thread->spl;
}

/**
* @brief Startet neuen Thread.
* @detail Der Stack wird präpariert, so dass der Thread die übergebene Funktion ausführt.
* @param *tcb Zeiger auf einen Thread-Control-Block
* @param *function Zeiger auf die auszuführende Funktion
* @param *stack Zeiger auf einen Speicherbereich, der vom Thread als Stack benutzt werden kann
* @param stacksize Größe des Stackbereichs
* @return void
*/
void MT_start_thread(MT_TCB *tcb,
					 PGM_VOID_P (*function)(void),
					 uint8_t  *stack,
					 uint16_t stacksize)

{

	/*
		TODO:
		-Muss höchstwertiges Adressbyte bei MT-killself() und *function
		 wirklich immer 0 sein??
		 
		-Cast-Warnings abstellen
	
	*/ 
	


	//PGM_VOID_P ist ein 16-Bit-Pointer. Reicht aus aum alle Funktionen 
	//im Flash zu addressieren, da word-aligned

	uint16_t sp_init;

	//Adresse der Kill-Thread-Fkt auf Stack
	stack[stacksize-1]=(uint8_t)&MT_killself;
	stack[stacksize-2]=(uint8_t)(  (uint16_t)&MT_killself >> 8);
	stack[stacksize-3]=0;   //(uint8_t)(  (uint16_t)&MT_killself >> 16);//sollte so gut wie immer 0 sein



	//Adresse des Funktionsbeginns auf den Stack
	stack[stacksize-4]=(uint8_t)function;
	stack[stacksize-5]=(uint8_t)((uint16_t)function>>8);
	stack[stacksize-6]=0x00;//(uint8_t)((uint16_t)function>>16);
	
	
	stack[stacksize-7]=0x00;//R1
	stack[stacksize-8]=0x00;//R0
	
	
	
	
	asm volatile("in r14, 0x3F\n"
				 "sts SR,r14");

	stack[stacksize-9]=SR; //Statusregister	

	sp_init=(uint16_t)(stack+stacksize - 7 - 32 -1); //3Byte f. RSA, 32Byte für Register, 1 Byte für SR
	tcb->sph=(uint8_t)(sp_init>>8);
	tcb->spl=(uint8_t)sp_init;

	tcb->next=current_thread->next;
	current_thread->next=tcb;

}

/**
* @brief Beendet Thread.
* @detail Thread wird aus der verketteten Liste ausgehängt, dann wird current_thread
* 			auf den nächsten Thread umgebogen. Dann wird mit einem Sprung zu REENTRY der 
* 			nächste Thread gestartet.
* @param void
* @return void
*/
void MT_killself(void)
{

	MT_TCB *p;
	current_thread->sph=0;	//Thread beendet
	current_thread->spl=0;

	for(p=current_thread;p->next!=current_thread;) //Vorgänger in der Thread-Liste finden
		p=p->next;

	p->next = current_thread->next;
	current_thread=p->next;	//Mit nächstem Thread in der Reihe weitermachen

	_SPH=current_thread->sph;
	_SPL=current_thread->spl;

	
	TCNT1=0;
	cli();

	asm("jmp REENTRY");

}






