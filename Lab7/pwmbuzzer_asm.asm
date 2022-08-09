; pwmled.asm: Starter code, assembly PWM-LED functions
; Lab 7, ECE 266, Spring 2021
; Created by Zhao Zhang

; Include C header files
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/sysctl.h","driverlib/gpio.h","driverlib/pin_map.h","driverlib/timer.h","pwmbuzzer.h"

                .text

; Field and any other declarations
;From table 11-1
WTIMER0          .field  WTIMER0_BASE
PORT            .field  GPIO_PORTC_BASE
PIN_ROUTE_1     .field  GPIO_PC5_WT0CCP1
;
; void buzzPWMSet(int period , int pulseWidth )
;
;
                .asmfunc
buzzPWMSet	    ; Set the period and duty cycle length for the buzzer
                ;   call TimerLoadSet(TIMER0, TIMER_B, pulsePeriod)
                ;   call TimerMatchSet(TIMER0, TIMER_B, pulseWidth)
				PUSH    {lr, r1, r0};

				LDR		r0, WTIMER0                 ; r0 = WTIMER0
                MOV     r1, #TIMER_B                ; r1 = TIMER_B
                LDR     r2, [sp, #0]	            ; r2 = period (pushed r0 in stack)
                BL      TimerLoadSet                ; call TimerLoadSet

                LDR     r0, WTIMER0                 ; r0 = WTIMER0
                MOV     r1, #TIMER_B                ; r1 = TIMER_B
                LDR     r2, [sp, #4]                ; r2 = pulseWidth (pushed r1 in stack)
                BL      TimerMatchSet

                ADD     sp, #8                     ; release the stack space for r1, r0
                POP     {pc}
                .endasmfunc

