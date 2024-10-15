/*
 * DacOutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "DacOutputDriver.h"


// Assign the DAC class members
DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* hdac, TIM_HandleTypeDef* htim, uint32_t DAC_CHANNEL, uint32_t DAC_ALIGN, uint8_t f, Queue* q){
	dac_port = hdac;
	tim_num = htim;
	dac_channel = DAC_CHANNEL;
	dac_alignment = DAC_ALIGN;
	freq = f;
	queue = q;
	follow_freq = 1;
}


// Set the output value of the DAC via DMA
void DACOutputDriver::update_DAC(/*bool follow_mode, uint32_t follow_f, */uint32_t* wave_LUT, uint16_t LUT_size){
	uint8_t f_move = 0;
	bool flag = queue->dequeue(&f_move, FREQ);

	if(flag){
		if(f_move == INC){
			if(freq >= F_KNOB_INTERVALS){
				freq = 1;
			}else{
				freq++;
			}
		}else if(f_move == DEC){
			if(freq <= 1){
				freq = F_KNOB_INTERVALS;
			}else{
				freq--;
			}
		}

		tim_num->Init.Period = (int)(8e+7/(LUT_size*(freq/(float)F_KNOB_INTERVALS)*10000))-1; // Set the period value of the corresponding DAC trigger timer given a desired frequency
		HAL_TIM_Base_Init(tim_num); // Reinitialize the timer
	}

	HAL_TIM_Base_Start(tim_num); // Start the timer
	HAL_DAC_Start_DMA(dac_port, dac_channel, wave_LUT, LUT_size, dac_alignment); // Output wave table via DMA for corresponding DAC channel
}


uint32_t DACOutputDriver::get_freq(void){
	return (int)((freq/(float)F_KNOB_INTERVALS)*10000);
}
