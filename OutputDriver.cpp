/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "OutputDriver.h"

OutputDriver::OutputDriver(DAC_HandleTypeDef* hdac, uint32_t DAC_CHANNEL, uint32_t DAC_ALIGN){
	dac_port = hdac;
	dac_channel = DAC_CHANNEL;
	dac_alignment = DAC_ALIGN;
}

void OutputDriver::config(){
	HAL_DAC_Init(dac_port);
	HAL_DAC_Start(dac_port, dac_channel);
}

void OutputDriver::update(int16_t wave_step){
	HAL_DAC_SetValue(dac_port, dac_channel, dac_alignment, wave_step);
}


