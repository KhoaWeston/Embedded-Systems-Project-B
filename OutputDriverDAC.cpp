/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <OutputDriverDAC.h>


// Assign the DAC class members
OutputDriverDAC::OutputDriverDAC(DAC_HandleTypeDef* hdac, TIM_HandleTypeDef* htim, uint32_t DAC_CHANNEL, uint32_t DAC_ALIGN){
	dac_port = hdac;
	tim_num = htim;
	dac_channel = DAC_CHANNEL;
	dac_alignment = DAC_ALIGN;
}


// Set the output value of the DAC
void OutputDriverDAC::update(uint32_t* wave_LUT, uint16_t LUT_size){
	HAL_TIM_Base_Start(tim_num);
	HAL_DAC_Start_DMA(dac_port, dac_channel, wave_LUT, LUT_size, dac_alignment);
}


