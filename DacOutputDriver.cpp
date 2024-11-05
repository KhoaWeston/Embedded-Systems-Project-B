/*
 * DacOutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "DacOutputDriver.h"


DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim1, TIM_HandleTypeDef* tim2, // @suppress("Class members should be properly initialized")
		uint32_t chan1, uint32_t chan2, uint32_t align, Queue* w_q, LUTQueue* lut_q, uint16_t size){
	ASSERT(dac != nullptr);
	ASSERT(tim1 != nullptr);
	ASSERT(tim2 != nullptr);
	ASSERT(w_q != nullptr);
	ASSERT(lut_q != nullptr);
	// TODO: ASSERT remaining parameters

	wave_queue = w_q;
	lut_wave_queue = lut_q;

	// Initialize DAC channels with setup parameters
	dac_ch1.initialize(dac, tim1, chan1, align, size, w_q, lut_q, &freq1_update_flag, LUT1, FREQ1_1);
	dac_ch2.initialize(dac, tim2, chan2, align, size, w_q, lut_q, &freq2_update_flag, LUT2, FREQ2_1);
}


// Set the output value of the DAC via DMA
void DACOutputDriver::update_DAC(void){
	// Update frequency of each DAC channel
	dac_ch1.update_freq(FREQ1_1);
	dac_ch2.update_freq(FREQ2_1);

	// Synchronize waves if either channel's frequency changes
	if(freq1_update_flag.is_flag_set() || freq2_update_flag.is_flag_set()){
		freq1_update_flag.reset_flag();
		freq2_update_flag.reset_flag();

		// Only sync if frequencies are same
		if(dac_ch1.get_freq() == dac_ch1.get_freq()){
			dac_ch1.restart_DAC();
			dac_ch2.restart_DAC();
		}
	}
}


void DACOutputDriver::IndDAC::initialize(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim, uint32_t chan,
		uint32_t align, uint16_t size, Queue* w_q, LUTQueue* lut_q, EventFlag* flag, uint8_t l_num, uint8_t f_num){
	dac_handle = dac;
	tim_handle = tim;
	dac_channel = chan;
	dac_alignment = align;

	lut_size = size;

	wave_queue = w_q;
	lut_wave_queue = lut_q;
	freq_update_flag = flag;

	(void)lut_wave_queue->dequeue(l_num, lut_point);	// Load the waveform data
	ASSERT(lut_point != nullptr);
	(void)wave_queue->dequeue(f_num, curr_freq_Hz);
	ASSERT(curr_freq_Hz > 0);

	reinitialize_timer();				// Set the initial frequency
	HAL_StatusTypeDef status = HAL_TIM_Base_Start(tim_handle); 	// Start the timer
	if(status != HAL_OK){
		NVIC_SystemReset();
	}
	restart_DAC();
}


void DACOutputDriver::IndDAC::update_freq(uint8_t f_num){
	// Adjust the timer frequency
	bool freq_updated = wave_queue->dequeue(f_num, curr_freq_Hz);
	if(freq_updated){
		ASSERT(curr_freq_Hz > 0);

		freq_update_flag->set_flag();
		reinitialize_timer();
	}
}


void DACOutputDriver::IndDAC::restart_DAC(void){
	(void)HAL_DAC_Stop_DMA(dac_handle, dac_channel);
	HAL_StatusTypeDef status = HAL_DAC_Start_DMA(dac_handle, dac_channel, lut_point, lut_size, dac_alignment);
	if(status != HAL_OK){
		NVIC_SystemReset();
	}
}


void DACOutputDriver::IndDAC::reinitialize_timer(void){
	uint32_t tim_period = SYS_CLOCK_FREQ / (lut_size * curr_freq_Hz * (TIM_PRESCALER + 1)) - 1;
	tim_handle->Instance->ARR = tim_period;
	tim_handle->Init.Period = tim_period;
}

uint32_t DACOutputDriver::IndDAC::get_freq(void){
	return curr_freq_Hz;
}

