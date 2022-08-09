;To include names declared in C
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/pin_map.h","driverlib/sysctl.h","driverlib/gpio.h","driverlib/adc.h","ras.h"

                .text
                .global rasRead
BASE     .field  ADC0_BASE
PERIPH	 .field  SYSCTL_PERIPH_ADC0

rasRead       PUSH    {LR, r0}            ; save the return address

			;initialize ADC
			LDR r0, PERIPH					; loads periph into r0
			BL SysCtlPeripheralEnable		; calls the function to enable adc

			; Trigger the sample sequence.
			LDR 	r0, BASE				; this loads the adc value into r0
			MOV 	r1, #0					; this puts 0 into r1
			BL 		ADCProcessorTrigger 	; this calls the ADC Processor Trigger function

			;wait until sample sequence has finished
while_loop	LDR 	r0, BASE				;  this loads the adc value into r0
			MOV 	r1, #0					; this puts 0 into r1
			MOV 	r2, #false				; this sets r2 to false
			BL 		ADCIntStatus			;
			CMP		r0, #0					; compare r0 and 0
			BEQ 	while_loop				; repeat loop if r0 == 0

			;read ADC Sequence data
			LDR 	r0, BASE				;  this loads the adc value into r0
			MOV 	r1, #0					; this puts 0 into r1
			POP 	{r2}					; this pops the initial parameter from rasRead into r2
			BL 		ADCSequenceDataGet   	; this calls the ADCSequenceData Get function which has 3 parameters, ADC0_BASE, 0, and &ui32Value

			;Clear interupt
			LDR 	r0, BASE				; this loads the adc value into r0
			MOV 	r1, #0					; this sets r1 to 0
			BL 		ADCIntClear				; this calls ADCIntClear function with parameters ACD0_Base and 0

			POP{PC}							; return


