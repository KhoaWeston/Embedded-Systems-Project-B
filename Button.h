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
		GPIO_TypeDef* port;
		uint16_t pin;
	public:
		/**
		 	* @brief constructor for class Button.
		 	* initializes port and pin variables.
		 	* @param x GPIOx pointer to a GPIO_TypeDef.
		 	* @param x GPIO_Pin accepts macro to set pin.
		 	* @return void.
		 	*/
		Button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
		/**
		 	* @brief retrieves the input pin value,
		 	* @param x void accepts no parameters.
		 	* @return pin a uint16_t value associated with the gpio pin.
		 	*/
		int16_t getPin(void);
		/**
			* @brief reads the value of the input gpio pin.
			* @param x void accepts no parameters
			* @return HAL_GPIO_ReadPin(port,pin) the GPIO_PinState of corresponding pin
			*/
		GPIO_PinState state(void);
};

#endif /* EMBEDDED_SYSTEMS_PROJECT_B_BUTTON_H_ */
