#include <avr/io.h>
#include <avr/interrupt.h>

/**
* Timer-ISR.
* Sichert Registerinhalte auf dem Stack, ruft schedule() auf und stellt Registerinhalte
* des neuen Threads her.
*/
.extern _SPH
.extern _SPL
.extern schedule

.global REENTRY


.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
	//Registerinhalte sichern.
	cli
	
	push r0
	push r1
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r28
	push r29
	push r30
	push r31

	in	r0, 0x3f //SR	
   	push	r0

	in r15, 0x3E//R15 <- SPH
	in r14, 0x3D//R14 <- SPL
	sts _SPL, r14 //_SPL <- R14
	sts _SPH, r15 //_SPH <- R15

	call schedule //Kontextwechsel -> _SPL + _SPH haben neuen Wert

	REENTRY: lds r15, _SPH  //R15 <- _SPH
	lds r14, _SPL 		//R14 <- _SPL
	out 0x3E, r15	//SPH <- R15
	out 0x3D, r14     //SPL <- R14


	pop	r0
    out	0x3f, r0	// SR wiederherstellen

			 
	pop r31
	pop r30
	pop r29
	pop r28
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0

	sei
	
	reti

