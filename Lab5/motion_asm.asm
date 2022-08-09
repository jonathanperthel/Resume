;
; Lab 4, ECE 266, spring 2021
;
; Assembly functions related to motion operations.

;

; To include names declared in C
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/pin_map.h","driverlib/sysctl.h","driverlib/gpio.h","buzzer.h"

                .text
                .global pirDetect

; GPIO peripheral, port base and pin mask value for the buzzer
; The pir sensor is assumed to be connected to J16, and then the pir's
; signal pin is connected to PC4.
MOTION_PORT     .field  GPIO_PORTC_BASE
MOTION_PIN      .equ    GPIO_PIN_4

;
; bool pirDetect(): This returns true if there is motion and false if there is not.
;
pirDetect        PUSH    {LR}            ; save the return address

                ; read the input to the GPIO pin that the pir sensor uses:
                ; GPIOPinRead(MOTION_PORT, MOTION_PIN)
                LDR     r0, MOTION_PORT
                MOV     r1, #MOTION_PIN
                BL      GPIOPinRead

                POP     {PC}            ; return

