;To include names declared in C
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/pin_map.h","driverlib/sysctl.h","driverlib/gpio.h","driverlib/adc.h","ranger.h","driverlib/timer.h"

                .text
                .global rangerDetect
TIMER_BASE 		.field  TIMER3_BASE
PORT            .field  GPIO_PORTB_BASE
TIMERB       	.field	TIMER_B
TIMER_EVENT		.field	TIMER_CAPB_EVENT


				;.asmfunc
rangerDetect	PUSH {lr}

				BL   startPulse ; calling function to start pulse from ranger.c

loop1			LDR  r0, TIMER_BASE;
				MOV  r1, #false;
				BL   TimerIntStatus;
				CMP  r0, #0;
				BEQ  loop1			; return to loop1

				LDR  r0, TIMER_BASE;
				MOV  r1, #TIMER_B;
				BL   TimerValueGet; determines rising edge
				PUSH {r0};

				LDR  r0, TIMER_BASE;
				MOV r1, #TIMER_CAPB_EVENT;
				BL TimerIntClear;



loop2			LDR  r0, TIMER_BASE;
				MOV  r1, #false;
				BL   TimerIntStatus;
				CMP  r0, #0;
				BEQ  loop2			; return to loop2


				LDR  r0, TIMER_BASE;
				MOV  r1, #TIMER_B;
				BL   TimerValueGet; determines falling edge
				PUSH {r0};

				LDR  r0, TIMER_BASE;
				MOV r1, #TIMER_CAPB_EVENT;
				BL TimerIntClear;


				; distance (mm) = (falling - rising) * (340(m/s) /2)*1000 /50000000
				;distance (mm) = (falling - rising) * (340/2) / 50000
				;distance (mm) = (falling - rising) * (170) / 50000
				;distance (mm) = (falling - rising) * (17) / 5000
				POP {r0, r1} ;
				SUB r0, r0, r1 ; subtracts falling edge from rising edge
				MOV r2, #17;
				MUL r0, r0, r2 ; multiplying by speed of sound
				MOV r3, #5000;
				UDIV r0, r0, r3;
				POP {PC};
				 ; .endasmfunc

