; Lab 6, ECE 266, spring 2021
; Starter code for the bonus part
;
; Created by Zhao Zhang

                .cdecls "stdbool.h","stdint.h","inc/hw_memmap.h","inc/hw_adc.h","driverlib/adc.h","ras.h"

                .text

;*******************************************************************
; Pointers to ADC I/O registers for Lab 6
; It uses ADC0 and the ADC's sample sequencer #0
; You may change it if you use ADC1 and/or another sequencer
;*******************************************************************

; ADC PSSI (ADC Processor Sample Sequence Initiate) register
ADC_PSSI_ptr    .field  ADC0_BASE + ADC_O_PSSI

; ADC RIS (ADC Raw Interrupt Status) register
ADC_RIS_ptr     .field  ADC0_BASE + ADC_O_RIS

; ADC SSFIFO0 (ADC Sample Sequence Result FIFO 0) register
ADC_SSFIFO0_ptr .field  ADC0_BASE + ADC_O_SSFIFO0

; ADC ISC (Interrupt Status and Clear) register
ADC_ISC_ptr     .field  ADC0_BASE + ADC_O_ISC

;******************************************************************
; Put your function here. You may revise this comment.
;
;
;******************************************************************

