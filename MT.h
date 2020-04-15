/**
* @file
* @brief Multithreading-Funktionen.
* @detail Diese Datei einthält die Implementierung der Multithreading-Funktionen.
*/
#ifndef __MT_H__
#define __MT_H__

#include <avr/io.h>
#include <avr/pgmspace.h>


#define MT_STACKSIZE 256

typedef struct _MT_TCB
{
	uint8_t sph;
	uint8_t spl;
	uint8_t ticksleft;
	uint8_t timeslice;
	struct _MT_TCB *next;
}MT_TCB;


//void schedule();
//extern volatile uint8_t _SPH, _SPL, SR;


void MT_init();
void MT_start_thread(MT_TCB *tcb,
					 PGM_VOID_P (*function)(void),
					 uint8_t  *stack,
					 uint16_t stacksize);

void MT_killself(void);
#endif //#ifndef __MT_H__
