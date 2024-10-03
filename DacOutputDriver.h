/*
 * DacOutputDriver.h
 *
 *  Created on: Oct 3, 2024
 *      Author: khoaw
 */

#ifndef INC_DACOUTPUTDRIVER_H_
#define INC_DACOUTPUTDRIVER_H_


#include "main.h"


class DACOutputDriver{
private:
	DAC_HandleTypeDef* dac_port; // Holds the specific DAC port
	TIM_HandleTypeDef* tim_num;
	uint32_t dac_channel; // Holds the corresponding DAC channel constant
	uint32_t dac_alignment; // Holds the corresponding DAC alignment constant
	uint16_t freq;

public:
	DACOutputDriver(DAC_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint16_t); // Initialize the DAC class members
	void update(uint32_t*, uint16_t); // Set the output value of the DAC
};


#endif /* INC_DACOUTPUTDRIVER_H_ */
