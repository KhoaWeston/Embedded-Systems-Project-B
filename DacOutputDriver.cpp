/*
 * DacOutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "DacOutputDriver.h"


DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim1, TIM_HandleTypeDef* tim2, // @suppress("Class members should be properly initialized")
		uint32_t chan1, uint32_t chan2, uint32_t align, Queue* q1, LUTQueue* q2, uint16_t s){
	wave_queue = q1;
	lut_wave_queue = q2;

	// Initialize DAC channels with setup parameters
	dac_ch1.initialize(dac, tim1, chan1, align, s, q1, q2, &freq1_update_flag, LUT1, FREQ1_1);
	dac_ch2.initialize(dac, tim2, chan2, align, s, q1, q2, &freq2_update_flag, LUT2, FREQ2_1);
}


// Set the output value of the DAC via DMA
void DACOutputDriver::update_DAC(void){
	// Update frequency of each DAC channel
	dac_ch1.update_freq(FREQ1_1);
	dac_ch2.update_freq(FREQ2_1);

	// Synchronize waves if either channel's frequency changes
	if(freq1_update_flag.is_flag_set() || freq2_update_flag.is_flag_set()){
		dac_ch1.restart_DAC();
		dac_ch2.restart_DAC();
	}
}


void DACOutputDriver::IndDAC::initialize(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim, uint32_t chan, uint32_t align, uint16_t s, Queue* q1, LUTQueue* q2, EventFlag* f, uint8_t l_num, uint8_t f_num){
	dac_handle = dac;
	tim_handle = tim;
	dac_channel = chan;
	dac_alignment = align;

	lut_size = s;

	wave_queue = q1;
	lut_wave_queue = q2;
	freq_update_flag = f;

	lut_wave_queue->dequeue(l_num, lut_point);	// Load the waveform data
	wave_queue->dequeue(f_num, curr_freq_Hz);

	reinitialize_timer();			// Set the initial frequency
	HAL_TIM_Base_Start(tim_handle); 		// Start the timer
	restart_DAC();
}


void DACOutputDriver::IndDAC::update_freq(uint8_t f_num){
	// Adjust the timer frequency
	bool freq_updated = wave_queue->dequeue(f_num, curr_freq_Hz);
	if(freq_updated){
		uint32_t tim_freq_max = SYS_CLOCK_FREQ/(TIM_PRESCALER*lut_size);
		ASSERT(0 < curr_freq_Hz && curr_freq_Hz <= tim_freq_max);

		freq_update_flag->set_flag();
		reinitialize_timer();
	}
}


void DACOutputDriver::IndDAC::restart_DAC(void){
	HAL_DAC_Stop_DMA(dac_handle, dac_channel);
	HAL_DAC_Start_DMA(dac_handle, dac_channel, lut_point, lut_size, dac_alignment);
}

void DACOutputDriver::IndDAC::reinitialize_timer(void){
	uint32_t tim_period = SYS_CLOCK_FREQ / (lut_size * curr_freq_Hz * TIM_PRESCALER) - 1;
	tim_handle->Instance->ARR = tim_period;
	tim_handle->Init.Period = tim_period;
}

