/*
 * DacOutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "DacOutputDriver.h"


// Assign the DAC class members
DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* hdac, TIM_HandleTypeDef* htim, uint32_t DAC_CHANNEL, uint32_t DAC_ALIGN, uint8_t f /*, Queue* q*/){
	dac_port = hdac;
	tim_num = htim;
	dac_channel = DAC_CHANNEL;
	dac_alignment = DAC_ALIGN;
	freq = (f/8.)*10000;
	/*queue = q;*/
}


// Set the output value of the DAC via DMA
void DACOutputDriver::update_DAC(uint32_t* wave_LUT, uint16_t LUT_size){
	// TODO: Dequeue from queue to get instructions for setting frequency

	tim_num->Init.Period = (int)(8e+7/(LUT_size*freq))-1; // Set the period value of the corresponding DAC trigger timer given a desired frequency
	HAL_TIM_Base_Init(tim_num); // Reinitialize the timer

	HAL_TIM_Base_Start(tim_num); // Start the timer
	HAL_DAC_Start_DMA(dac_port, dac_channel, wave_LUT, LUT_size, dac_alignment); // Output wave table via DMA for corresponding DAC channel
}


uint32_t DACOutputDriver::get_freq(void){
	return freq;
}
