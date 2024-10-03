/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <DacOutputDriver.h>


// Assign the DAC class members
DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* hdac, TIM_HandleTypeDef* htim, uint32_t DAC_CHANNEL, uint32_t DAC_ALIGN, uint16_t f){
	dac_port = hdac;
	tim_num = htim;
	dac_channel = DAC_CHANNEL;
	dac_alignment = DAC_ALIGN;
	freq = f;
}


// Set the output value of the DAC
void DACOutputDriver::update(uint32_t* wave_LUT, uint16_t LUT_size){
	tim_num->Init.Period = (int)(8e+7/(128*freq))-1;
	HAL_TIM_Base_Init(tim_num);

	HAL_TIM_Base_Start(tim_num);
	HAL_DAC_Start_DMA(dac_port, dac_channel, wave_LUT, LUT_size, dac_alignment);
}


