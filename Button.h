/*
 * Button.h
 *
 *  Created on: Oct 8, 2024
 *      Author: kcmartin
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_BUTTON_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_BUTTON_H_

#include "main.h"
#include <stdint.h>
#include <stdio.h>

class Button{
	private:
		GPIO_Typedef* port;
		uint16_t pin;
	public:
		Button(GPIO_TypeDef* givenPort, uint16_t givenPin);
		int16_t getPin(void);
		GPIO_PinState state(void);
};

#endif /* EMBEDDED_SYSTEMS_PROJECT_B_BUTTON_H_ */
