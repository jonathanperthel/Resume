/*
 * pwmbuzzer.c
 *
 *  Created on: Mar 9, 2021
 *      Author: Jonathan Perthel
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include "pwmbuzzer.h"

// Pin usage: Grove base port J17, Tiva C PC5 (Port C, Pin 5)
#define BUZZER_PERIPH   SYSCTL_PERIPH_GPIOC
#define BUZZER_PORT     GPIO_PORTC_BASE
#define BUZZER_PIN      GPIO_PIN_5

#define WTIMER0         WTIMER0_BASE
#define PORTC           GPIO_PORTC_BASE

// Initialize the buzzer
void buzzerInit()
{
    // Enable Wide Timer 0
    //Must use wide timer since using J17 which is PC5 which uses WTIMER0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0); //for this case we will use the wide timer which has a greater amount of bits

    // Enable the port peripheral used by the buzzer
    //This has to be called since buzzer is peripheral
    //This is from Lab 4
    SysCtlPeripheralEnable(BUZZER_PERIPH);


    // Connect pins to those timers
    GPIOPinTypeTimer(PORTC, BUZZER_PIN);
    GPIOPinConfigure(GPIO_PC5_WT0CCP1); //from table 11-1


    // Select PWM for Timer 0 sub-Timer B
    TimerConfigure(WTIMER0, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM));


    // Invert the PWM waveform, so that the Match register value is the pulse width.
    // Otherwise, the pulse width will be (Load value) - (Match value).
    // The inversion is done by enabling output inversion on the PWM pins.
    TimerControlLevel(WTIMER0, TIMER_B, true /* output inversion */);

    // Enable the Timer 0's TimerB
    TimerEnable(WTIMER0, TIMER_B);

}
