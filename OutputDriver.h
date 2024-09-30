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
	DAC_HandleTypeDef* dac_port;
	uint32_t dac_channel;
	uint32_t dac_alignment;

public:
	OutputDriver(DAC_HandleTypeDef*, uint32_t, uint32_t); // Stores the GPIO pin mappings
	void config();
	void update(int16_t);
};


#endif /* EMBEDDED_SYSTEMS_PROJECT_B_OUTPUTDRIVER_H_ */
