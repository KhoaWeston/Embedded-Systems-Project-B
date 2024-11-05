/*
 * Button.cpp
 *
 *  Created on: Oct 8, 2024
 *      Author: kcmartin
 */

#include "Button.h"

Button::Button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	port = GPIOx;
	pin = GPIO_Pin;
}

int16_t Button::getPin(void){
	return pin;
}

GPIO_PinState Button::state(void){
	return HAL_GPIO_ReadPin(port,pin);
}

