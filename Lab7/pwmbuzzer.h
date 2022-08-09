/*
 * pwmbuzzer.h
 *
 *  Created on: Mar 9, 2021
 *      Author: perth
 */

#ifndef PWMBUZZER_H_
#define PWMBUZZER_H_

#include <stdint.h>

// Initialize the buzzer
void buzzerInit();

//sets the pwn paramaters for the buzzer
void buzzPWMSet(int pulsePeriod, int maxPulseWidth);


#endif /* PWMBUZZER_H_ */
