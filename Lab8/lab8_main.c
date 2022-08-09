/*
 * lab8_main.c
 *
 *  Created on: Mar 16, 2021
 *
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
#include "ranger.h"
#include "seg7.h"
#include "seg7digit.h"
#include "launchpad.h"

//the running states of sw1
enum {
    Run, Pause
}  sysState = Pause;

enum {
    mm, inch
}  measureType = inch;

// The initial state of the 7-segment display: "00:00" with colon on
seg7Display_t seg7Display = {
    0, 0, 0, 0, 1
};




void checkPushButton(uint32_t time)
{
    uprintf("%s\n\r", "checkPushButton Called");
    int code = pbRead();

    switch (code) {
    case 1:                         // SW1 is the START/PAUSE button

        if (sysState == Pause) //If the stopwatch is in the paused state, then it will run again once clicked
        {
            sysState = Run;
        }
        else if (sysState == Run) //If the stopwatch is in the run state, then it will pause again once clicked
        {
            sysState = Pause;
        }

        break;

    case 2:                         // SW2 is used to switch between inches and millimeters

        if (measureType == mm) // if the button is clicked it switches to mm
        {
            measureType = inch;
        } else
        {
            measureType = mm;
        }


        break;


    }

    schdCallback(checkPushButton, time + 100);
}

void DistanceUpdate(uint32_t time)                          // The scheduled time
{
    //uprintf("%s\n\r", "distance update Called");
    int distance = 150;
    distance = rangerDetect();
    // Update clock and display, only if the stopwatch is running
    uprintf("%d\n\r", distance);
    if (sysState == Run) {
        int d1 = 0;
           int d2 = 0;
           int d3 = 0;
           int d4 = 0;
        //uprintf("%s\n\r", "system state is on");
        if (measureType == mm) { // if mm mode
           seg7Display.colon= 0;

           if (distance >= 9999) { // if value greater than 9999
               seg7Display.d1= 9;
               seg7Display.d2= 9;
               seg7Display.d3= 9;
               seg7Display.d4= 9;
           } else {
               //uprintf("%s\n\r", "update display digits");
               if (distance / 1000 == 0){ // to remove leading zero
                   //uprintf("%s\n\r", "distance less than 1000");

                   d4 = 10;
                   if (distance / 100 == 0){
                       d3 = 10;
                       if (distance / 10 == 0){
                           d2 = 0;
                           d1 = distance % 10;
                       } else {
                           d2 = distance / 10;
                           d1 = distance % 10;
                       }
                   } else {
                       d3 = distance / 100;
                       distance = distance % 100;
                       d2 = distance / 10;
                       d1 = distance % 10;
                   }
               } else {
                   d4 = distance / 1000;
                   distance = distance % 1000;
                   d3 = distance / 100;
                   distance = distance % 100;
                   d2 = distance / 10;
                   d1 = distance % 10;
               }
           }
     }
       if (measureType == inch) { // if inch mode
           seg7Display.colon= 1;
           distance = distance / 25.4; //inches = mm/25.4
           if (distance/12 == 0){ //if distance is less than a foot
               d3= 10;
               d4= 10;
               if (distance % 12 < 10){
                   d2 = 10;
                   d1 = distance % 10;
               } else {
                   d2 = distance / 10;
                   d1 = distance % 10;
               }
           } else {
               if (distance / 120 == 0){
                   d4 = 10;
                   d3 = distance /12;
                   distance = distance % 12;
                   d2 = distance /10;
                   d1 = distance % 10;
               } else {
                   d4 = distance / 120;
                   distance = distance % 120;
                   d3 = distance / 12;
                   distance = distance % 12;
                   d2 = distance / 10;
                   d1 = distance % 10;
               }

           }
       }
       //updates seven segment display values
       seg7Display.d4 =  d4;
       seg7Display.d3 =  d3;
       seg7Display.d2 =  d2;
       seg7Display.d1 =  d1;


    // Call back after 10 milliseconds
    }

    seg7DigitUpdate(&seg7Display);
    schdCallback(DistanceUpdate, time + 100); //use same function as in line 32
}


int main(void)
{
    lpInit();
    seg7Init();
    rangerInit();

    uprintf("%s\n\r", "Lab 8: Distance Measurement");

    // Update the clock display
    seg7DigitUpdate(&seg7Display);

    // Schedule the first callback events for checkPushbutton and distance update
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(checkPushButton, 1000);
    schdCallback(DistanceUpdate, 1002);

    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}

