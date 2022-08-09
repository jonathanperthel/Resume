/*
 * main.c: Starter code for Lab 7, Spring 2021
 *
 * Created by Zhao Zhang
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "launchpad.h"
#include "ras.h"
#include "pwmled.h"
#include "pwmbuzzer.h"

// LED-related constant
#define LED_PWM_PERIOD          5000
#define LED_MAX_PULSE_WIDTH     100

#define MAX_BUZZ_PWM_PERIOD          ((int) (50000000/261.63)) //191402
#define MIN_BUZZ_PWM_PERIOD          ((int) (50000000/523.25)) // 95556
#define BUZZ_DUTY_CYCLE                25


// ON/Off state type
typedef enum {On, Off} OnOff_t;


// Buzzer state
static OnOff_t buzzerState = On;

/*
 * Task 1: Play a sine pattern on LED
 */

typedef struct
{
    int pwmPeriod;              // PWM period for LED
    int maxPulseWidth;          // maximum pulse width
} LedState_t;

static LedState_t led =
        { LED_PWM_PERIOD /* PWM Period */, LED_MAX_PULSE_WIDTH * 0.4 /* Multiply factor */};

static LedState_t buzz =
        { MAX_BUZZ_PWM_PERIOD /* PWM Period */, MAX_BUZZ_PWM_PERIOD/*max pulse width = max period */};

// A sine function that uses degree as input. It converts degree to radian and then
// calls the sin() function of the C library.
static inline double sine(uint16_t degree)
{
    double radian = 2 * M_PI * ((double) (degree % 360) / 360);
    return sin(radian);
}

// LED playing callback function
void callbackLedPlay(uint32_t time)
{
    static uint16_t angle = 0;      // the degree of angle, used for calculating sine value
    int delay = 5;                  // the callback delay in ms

    // Calculate PWM parameters for red, blue, and green sub-LEDs using sine function.
    // Use phase shift of 60, 30, and 0 degrees for red, blue, and green
    int pulseWidthRed = sine(angle + 72) * led.maxPulseWidth;
    int pulseWidthBlue = sine(angle + 36) * led.maxPulseWidth;
    int pulseWidthGreen = sine(angle) * led.maxPulseWidth;

    // Set the PWM parameters for the three sub-LEDs
    ledPwmSet(led.pwmPeriod, pulseWidthRed, pulseWidthBlue, pulseWidthGreen);

    // Advance the angle by one degree, so a play period is 360 * 5 = 1800 ms
    angle = (angle + 1) % 360;

    // Schedule the next callback
    schdCallback(callbackLedPlay, time + delay);
}

void callbackBuzzerPlay(uint32_t time)                    // the scheduled time
{
    uint32_t delay = 10;


        // Turn the buzzer on and off alternatively
        // Adjust the time values to control the sound intensity
        switch (buzzerState)
        {
        //this case turns the buzzer off by setting the pulse width and frequency to 0
        case On:
            buzzPWMSet(0, 0);
            buzzerState = Off;
            delay = 2800;                           // off for 2800 ms
            break;
        //this case turns the buzzer on by setting the pwm period and the max pulse width equal to
        case Off:
            buzzPWMSet(buzz.pwmPeriod, buzz.maxPulseWidth);
            buzzerState = On;
            delay = 200;                            // on for 200 ms
            break;
        }

    // schedule the next callback
    schdCallback(callbackBuzzerPlay, time + delay);
}

void ADCOutput(uint32_t time)
{
    uint32_t delay = 100;        // the default delay for the next checking
    uint32_t rasInput[2];        //this creates an array with 2 elements
    rasRead(rasInput);          //this reads the inputs from the two ras inputs and puts them into the array
    uint32_t ledPWM = ((rasInput[0]*100)/4095);     //this converts the ras input to a value between 1-100
    led.maxPulseWidth = LED_MAX_PULSE_WIDTH - ledPWM;//this sets the max pulse width of the LED by subtracting the ratio from the max

    uint32_t buzzPWM = ((rasInput[0]*100)/4095)*(MAX_BUZZ_PWM_PERIOD*BUZZ_DUTY_CYCLE);  //this sets the ratio of the max pulse width for the buzzer
    buzz.maxPulseWidth = ((MAX_BUZZ_PWM_PERIOD*BUZZ_DUTY_CYCLE) - (buzzPWM/100))/100;   //this sets the max pulse width of the buzzer by subtracting the ratio from the max
    //uprintf("%u\n\r",  buzz.maxPulseWidth);

    uint32_t buzzPeriod = (((rasInput[1]*100)/4095)*(MAX_BUZZ_PWM_PERIOD-MIN_BUZZ_PWM_PERIOD))/100; //this sets the ratio for the buzzer period based on the minimum and maximum values
    buzz.pwmPeriod = buzzPeriod + MIN_BUZZ_PWM_PERIOD;  //this sets the period for the buzzer
    //uprintf("%u\n\r",  buzz.pwmPeriod);

    schdCallback(ADCOutput, time + delay);
}

void main(void)
{
    lpInit();
    ledPwmInit();
    rasInit();
    buzzerInit();

    // Schedule the first callback events for for the ADCOutput, callbackLedPlay, and callbackBuzzerPlay
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(ADCOutput, 1000);
    schdCallback(callbackLedPlay, 1002);
    schdCallback(callbackBuzzerPlay, 1004);

    // Loop forever
    while (true)
    {
        schdExecute();
    }
}
