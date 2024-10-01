/*
 * OutputDriver.h
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_

#include "main.h"


class OutputDriverDAC{
private:
	DAC_HandleTypeDef* dac_port; // Holds the specific DAC port
	TIM_HandleTypeDef* tim_num;
	uint32_t dac_channel; // Holds the corresponding DAC channel constant
	uint32_t dac_alignment; // Holds the corresponding DAC alignment constant

public:
	OutputDriverDAC(DAC_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t); // Initialize the DAC class members
	void update(uint32_t*, uint16_t); // Set the output value of the DAC
};


#endif /* EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_ */
