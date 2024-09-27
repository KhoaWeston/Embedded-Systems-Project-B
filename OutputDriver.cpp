/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "OutputDriver.h"

OutputDriver::OutputDriver(GPIO_TypeDef* givenPort0, GPIO_TypeDef* givenPort1){
	port0 = givenPort0; // GPIOA
	port1 = givenPort1; // GPIOB
}

void OutputDriver::update(int16_t){
	;
}


