/*
 * lab5_main.c: ECE 266 Lab 5, spring 2021, Motion Detection Alarm (interrupt-based)
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/pin_map.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/adc.h>
#include "launchpad.h"
#include "ras.h"
#include "seg7.h"
#include "seg7digit.h"


seg7Display_t seg7Display = {
    0, 0, 0, 0, 1
};

void ADCOutput(uint32_t time)
{

    uint32_t delay = 100;        // the default delay for the next checking
    uint32_t rasInput[2];
    rasRead(rasInput);
    uprintf("%u\n\r", rasInput[1]);

    //for first input channel
    int c = (rasInput[0]*100 / 4095) * 99; //reads from first channel in the ADC
    int c1 = c / 1000;          //equation for the tens digit.
    int c2 = (c/100) % 10;      //equation for the ones digit. used modulo 10 to get the remainder for the ones

    //for second input channel
    int s = (rasInput[1]*100 / 4095) * 99; //reads from second channel in the ADC
    int s1 = s / 1000;         //equation for the tens digit
    int s2 = (s/100) % 10;     //equation for the ones digit. used modulo 10 to get the remainder for the ones

    seg7Display.d1= s2; //for leftmost digit on the analog display
    seg7Display.d2= s1; //for center left digit on the analog display
    seg7Display.d3= c2; //for center right digit on the analog display
    seg7Display.d4= c1; //for rightmost digit on the analog display
    seg7DigitUpdate(&seg7Display);


   // schedule the next callback
     schdCallback(ADCOutput, time + delay);

}

/*
 * The main function
 */
int main(void)
{
    lpInit();
    seg7Init();
    rasInit();

    // Update the clock display
    seg7DigitUpdate(&seg7Display);


    // Print out a start message
    uprintf("%s\n\r", "Lab 6 starts");

    schdCallback(ADCOutput, 1000);

    while (true)
      {
          schdExecute();
      }


}

