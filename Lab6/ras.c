


#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/adc.h>
#include "launchpad.h"
#include "ras.h"

void rasInit(){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable the first sample sequencer to capture the value of channel 0 when
    // the processor trigger occurs.
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_CH7); //for first channel in the ADC
    //first parameter is for the base address of the ADC module, second parameter the sample sequence number
    //and the step to be configured are both 0. The last parameter has an OR statement connecting the two. The one ending in IE
    //stands for interrupt when the step is complete. CH7 stands for the channel that is sampled.
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH6); //for second channel in the ADC
    //first parameter is for the base address of the ADC module, second parameter the sample sequence number is 0
    //and the step to be configured is 1. The last parameter has an OR statement connecting the three options. The one ending in IE
    //stands for interrupt when the step is complete. The END stands for last step in the sequence. The CH^ stands for the channel
    //that is sampled.

    ADCSequenceEnable(ADC0_BASE, 0);
}

