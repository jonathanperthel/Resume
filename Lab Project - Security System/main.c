

/**
 * main.c
 */

//Buzzer connected to J17
//PIR Sesnor connected to J16
//Seven Segment Display connected to J10
//Ras connected to J5
//Need to initialize pushbuttons
//turn on the LEDS on board

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
#include "buzzer.h"
#include "motion.h"

// ON/Off state type
typedef enum {On, Off} OnOff_t;

// System running state
volatile static OnOff_t sysState = Off;

// Alarm state
volatile static OnOff_t alarmState = Off;

// Buzzer state
volatile static OnOff_t buzzerState = Off;

// Passcode Enter state
volatile static OnOff_t passState = Off;

// Light cycle state
volatile static OnOff_t passChanging = Off;

bool red = false;
bool blue = false;
bool green = false;

//for user to switch to next passcode digit
volatile static OnOff_t passStateMove = Off;

seg7Display_t seg7Display = {
    10, 10, 10, 10, 0
};

seg7Display_t passcode = {
    4, 3, 2, 1, 0
};

// this variable represents the digit of the 4-digit display being changed
uint32_t digitControl = 4;

// Callback function for playing the buzzer
void callbackBuzzerPlay(uint32_t time)                    // the scheduled time
{
    uint32_t delay = 10;

    if (sysState == On) {
        if (alarmState == On)
        {
            // Turn the buzzer on and off alternatively
            // Adjust the time values to control the sound intensity
            switch (buzzerState)
            {
            case On:
                buzzerOff();
                buzzerState = Off;
                delay = 2988;                           // off for 2988 ms
                break;

            case Off:
                buzzerOn();
                buzzerState = On;
                delay = 12;                            // on for 12 ms
                break;
            }
        }
    }

    // schedule the next callback
    schdCallback(callbackBuzzerPlay, time + delay);
}


//this function checks the passcode with what the user inputed.
//depending on the system state different responses will occur
void checkPasscode () {
    //this checks each of the four inputted digits with the corresponding digit in the passcode
    if (seg7Display.d4 == passcode.d4){
        if (seg7Display.d3 == passcode.d3){
            if (seg7Display.d2 == passcode.d2){
                if (seg7Display.d1 == passcode.d1){

                    //this loop is responsible for flashing the LED green when password correct
                    int time = 0;
                    while (time < 2000000) {
                        ledTurnOnOff(false, false, true); //red, blue, green
                        waitUs(250000);
                        ledTurnOnOff(false, false, false); //red, blue, green
                        waitUs(250000);
                        time = time + 500000;
                    }
                    //this if statement is used for turning the system off
                    if (sysState == On){
                    sysState = Off;
                    alarmState = Off;
                    ledTurnOnOff(false /* red */, false /* blue */, false /* green */); //all led's turn off
                    buzzerOff();
                    } else {
                        //this else statement is used for turning the system on when passcode correct
                        sysState = On;
                        if (alarmState == On) {
                            ledTurnOnOff(true, false, false); //red, blue, green
                        } else {
                            ledTurnOnOff(false, false, true); //red, blue, green
                        }
                    }
                }
            }
        }
    }
    //this if statement is true if the inputted code is incorrect
    if (seg7Display.d4 != passcode.d4  || seg7Display.d3 != passcode.d3 || seg7Display.d2 != passcode.d2 || seg7Display.d1 != passcode.d1) {
        buzzerOn();
        int time = 0;
        //this loop flashes the red LED if the passcode is incorrect
        while (time < 2000000) {
            ledTurnOnOff(true, false, false); //red, blue, green
            waitUs(250000);
            ledTurnOnOff(false, false, false); //red, blue, green
            waitUs(250000);
            time = time + 500000;
        }
        buzzerOff();
    }
    //this turns the display off when the function is done checking the passcode
    seg7Display.d4 = 10;
    seg7Display.d3 = 10;
    seg7Display.d2 = 10;
    seg7Display.d1 = 10;
    seg7DigitUpdate(&seg7Display);
    passState = Off;
}

// The ISR (interrupt service routine) for checking the pushbuttons
void pbISR()
{
    // Last time pushbutton was pushed
    static uint32_t lastTime = 0;

    // IMPORTANT: Clear interrupt, otherwise the interrupt handler will be executed forever
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // Read the pushbutton
    int code = pbRead();

    // Get the current system time
    uint32_t time = sysTimeGet();

    // De-bouncing: If a key was pushed within 250 ms, ignore this push
    if (time < lastTime + 250)
        return;

    // Process the pushbutton input
    switch (code)
    {
    case 1: //if PB1 is pressed
                passState = On; //password entering state is on

                //4-digit display is turned on
                seg7Display.d4 = 0;
                seg7Display.d3 = 0;
                seg7Display.d2 = 0;
                seg7Display.d1 = 0;
                seg7DigitUpdate(&seg7Display);

                //if the password reset mode is activated
                if (passStateMove == On){
                    passChanging = On;
                    red = true;
                }
                break;
//            }


      case 2:                     // SW2: change the digit of display
          //if the password entering mode is on
          if (passState == On){
              //if the digit currrently being controlled isn't the rightmost digit
              //then decrement the digit by 1
             if (digitControl > 1) {
                 digitControl--;

               //if the digit is the right most digit
             } else {
                 digitControl = 4; //sets the digit being controlled back to 4
                 //if the password isn't being reset then check the passcode
                 if (passStateMove == Off){
                     checkPasscode();

                     //if the passcode is being reset, then turn off the display and flash blue LED
                 } else {
                     seg7Display.d4 = 10;
                     seg7Display.d3 = 10;
                     seg7Display.d2 = 10;
                     seg7Display.d1 = 10;
                     seg7DigitUpdate(&seg7Display);
                     int time = 0;
                     //this loop flashes the blue LED for 2 seconds
                     while (time < 2000000) {
                         ledTurnOnOff(false, true, false); //red, blue, green
                         waitUs(250000);
                         ledTurnOnOff(false, false, false); //red, blue, green
                         waitUs(250000);
                         time = time + 500000;
                     }
                     ledTurnOnOff(false, false, false); //red, blue, green
                     passStateMove = Off;
                     passState = Off;
                     passChanging = Off;
                 }
             }
             //if the system state is off and the password reset is off, then the password reset mode is activated
          }else if (sysState == Off && passStateMove == Off) {
              passStateMove = On;
          }
    }

    // record the time to check for de-bouncing next time
    lastTime = time;
}



void pirISR() {
        // Last time motion was detected
        static uint32_t lastTime = 0;
        // IMPORTANT: Clear interrupt, otherwise the interrupt handler will be executed forever
        GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

        // Get the current system time
         uint32_t time = sysTimeGet();

        // De-bouncing: If a key was pushed within 250 ms, ignore this push
        if (time < lastTime + 250)
           return;

        //if motion is detected and the system state is on, then the alarm state will remain on until system turned off
        if (pirDetect() && sysState == On){
               alarmState = On;
               ledTurnOnOff(true /* red */, false /* blue */, false /* green */);
           }

        // record the time to check for de-bouncing next time
           lastTime = time;
}

/*
 * Configure the interrupt system
 */
void intrInit()
{
    // Set up ISR for pushbuttons: Port F, pin 0 (SW1) and pin 4 (SW2)
    GPIOIntRegister(GPIO_PORTF_BASE, pbISR); // register the interrupt handler
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, // interrupt on falling edge, note that SW1 and SW2 are active low
                   GPIO_FALLING_EDGE);
    IntPrioritySet(INT_GPIOF, 1); // set interrupt level to 1 (0 is the highest for programmable interrupts)
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); // enable interrupts on SW1 and SW2 input

    // Set up pirISR: Port C, pin 4
    GPIOIntRegister(GPIO_PORTC_BASE, pirISR); // register the motion detector
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_BOTH_EDGES); // interrupt on both edges, pir sensor is active high
                                                                  // we want to check when there is a rising edge and falling edge
                                                                  //because we want the pirInit function to be called when the signal changes
                                                                  // from high to low or low to high (to update the alarm state and LEDs
    IntPrioritySet(INT_GPIOC, 0); // set interrupt level to 0 (0 is the highest for programmable interrupts)
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4); // enable interrupts on pir sensor input


}

void ADCOutput(uint32_t time)
{

    uint32_t delay = 100;        // the default delay for the next checking
    uint32_t rasInput[1];
    rasRead(rasInput);
    //uprintf("%u\n\r", rasInput[0]);

    //for first input channel
    int c = (rasInput[0]*100 / 4095) * 9; //reads from first channel in the ADC
    //uprintf("%d\n\r", c);

    c = c / 100;
    //this sets the proper digit of the 4-digit display depending on the value of digitControl which was set in pbISR()
    if (passState == On){
        if (digitControl == 4){
            seg7Display.d4 = c;
        } else if (digitControl == 3){
            seg7Display.d3 = c;
        } else if (digitControl == 2) {
            seg7Display.d2 = c;
        } else {
            seg7Display.d1 = c;
        //if the password reset mode is activated, then the stored password is changed to the value on the display
      } if (passStateMove == On){
          if (digitControl == 4) {
          passcode.d4 = c;
          } if (digitControl == 3) {
          passcode.d3 = c;
          } if (digitControl == 2) {
          passcode.d2 = c;
          } if (digitControl == 1){
          passcode.d1 = c;
          }
        }
    } else {
        //turn off 4-digit display
        seg7Display.d4 = 10;
        seg7Display.d3 = 10;
        seg7Display.d2 = 10;
        seg7Display.d1 = 10;

    }
    seg7DigitUpdate(&seg7Display);

   // schedule the next callback
     schdCallback(ADCOutput, time + delay);

}

/*
 * The main function
 */

void ledCycle(uint32_t time) {
    //uprintf("%s\n\r", "light cycle");

    int32_t delay = 250;        // the default delay for the next checking
    //if the password is currently being reset
    //alternate the colors from red to blue to green
    if (passChanging == On) {
        if (red == true) {
            red = false;
            blue = true;
        } else if (blue == true){
            green = true;
            blue = false;
        } else {
            red = true;
            green = false;
        }
        ledTurnOnOff(red /* red */, blue /* blue */, green /* green */);
    }

    // schedule the next callback
    schdCallback(ledCycle, time + delay);
}


int main(void)
{
    lpInit(); // pushbutton initialize
    buzzerInit();//buzzer initialize
    pirInit();//pir sensor initialize
    intrInit();//interrupt initialize
    seg7Init();//seven segment display initialize
    rasInit(); //ras initialize


    // Print out a start message
    uprintf("%s\n\r", "Lab Project");

    // All led's turn off
    ledTurnOnOff(false /* red */, false /* blue */, false /* green */);

    //schedule first call of ledCycle
      schdCallback(ledCycle, 1000);

    // Schedule the first callback events
    schdCallback(callbackBuzzerPlay, 1002);

    //Schedule the first call of ADC ouput
    schdCallback(ADCOutput, 1004);



    // Run the callback scheduler
    while (true)
    {
        schdExecute();

        // Put Tiva C into sleep until the next interrupt happens
        __asm("    wfi");
    }
}

