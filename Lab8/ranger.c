/*
 * ranger.c
 *
 *  Created on: Mar 16, 2021
 *      Author: joebi
 */
#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include "launchpad.h"
#include "ranger.h"

//J15 uses PB3
#define GPIO_PORT_PERIPH    SYSCTL_PERIPH_GPIOB // The peripheral def of Port B
#define PORTB               GPIO_PORTB_BASE     // J15 uses port B

#define RANGER_PIN          GPIO_PIN_3          //Ranger connected to PIN 3
#define TIMER3              TIMER3_BASE         //Ranger uses timer 3


void rangerInit()
{
       // Enable Timer 0
       //Must use wide timer since using J15 which is PB3 which uses TIMER1
       SysCtlPeripheralEnable(GPIO_PORT_PERIPH); //for this case we will use the timer which has a greater amount of bits

       // Enable the port peripheral used by the buzzer
       SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);



       // Select mode for rising and falling edges for Timer 3 sub-Timer B
       TimerConfigure(TIMER3, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_CAP_TIME_UP));


       // Need to look at both the rising edge and the falling edge
       TimerControlEvent(TIMER3, TIMER_B, TIMER_EVENT_BOTH_EDGES );


       // Enable the Timer 0's TimerB
       TimerEnable(TIMER3, TIMER_B);


}

void startPulse(){

    GPIOPinTypeGPIOOutput(PORTB, RANGER_PIN); // sets the gpio pin to output

    GPIOPinWrite(PORTB, RANGER_PIN, 0); // sets the output low
    waitUs(2); //waits 2 microseconds
    GPIOPinWrite(PORTB, RANGER_PIN, RANGER_PIN); //this sets the output high
    waitUs(5); //waits for 5 microseconds
    GPIOPinWrite(PORTB, RANGER_PIN, 0); // sets the output low

    GPIOPinTypeTimer(PORTB, RANGER_PIN);
    GPIOPinConfigure(GPIO_PB3_T3CCP1); //from table 11-1


    TimerIntClear(TIMER3, TIMER_CAPB_EVENT); // we think we should use Capture B event for the flag

}


