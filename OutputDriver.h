/*
 * OutputDriver.h
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_

#include "main.h"

class OutputDriver{
private:
	GPIO_TypeDef* port0; // for GPIOA
	GPIO_TypeDef* port1; // for GPIOB
public:
	OutputDriver(GPIO_TypeDef*, GPIO_TypeDef*); // Stores the GPIO pin mappings
	void update(int16_t);
};


#endif /* EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_ */
