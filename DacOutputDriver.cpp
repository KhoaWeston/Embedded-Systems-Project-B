/*
 * DacOutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "DacOutputDriver.h"


DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim, uint32_t chann, uint32_t align, uint8_t fr, WaveQueue* q, EventFlag* fl1, EventFlag* fl2, EventFlag* fl3){ // @suppress("Class members should be properly initialized")
	dac_handle = dac;
	timer_handle = tim;
	dac_channel = chann;
	dac_alignment = align;
	curr_freq = fr;

	queue = q;
	dac_update_flag = fl1;
	follow_mode_flag = fl2;
	freq_update_flag = fl3;

	follow_mode_active = false;
	follow_mode_freq = 1;
}


void DACOutputDriver::initialize_timer(uint16_t LUT_SIZE){
	uint32_t freq_Hz = (curr_freq / (float)FREQ_KNOB_STEPS) * MAX_FREQ;
    timer_handle->Init.Period = static_cast<uint32_t>(SYS_CLOCK_FREQ / (LUT_SIZE * freq_Hz)) - 1;
    HAL_TIM_Base_Init(timer_handle); // Reinitialize the timer with a new period
}


// Set the output value of the DAC via DMA
void DACOutputDriver::update_DAC(bool follow_mode, uint32_t follow_f, uint32_t* wave_LUT, uint16_t LUT_SIZE){
	uint8_t freq_change = 0;
	bool freq_updated = queue->dequeue(&freq_change, FREQ);

	// Adjust the timer frequency
	if(freq_updated){
		if(freq_change == INC){
			curr_freq = (curr_freq >= FREQ_KNOB_STEPS) ? 1 : curr_freq+1;
		}else if(freq_change == DEC){
			curr_freq = (curr_freq <= 1) ? FREQ_KNOB_STEPS : curr_freq-1;
		}

		freq_update_flag->set_flag();
		uint32_t freq_Hz = (curr_freq / (float)FREQ_KNOB_STEPS) * MAX_FREQ;
		reinitialize_timer(freq_Hz, LUT_SIZE);
	}

	// Turn on or off follow mode if change is sensed from wave class
	if(follow_mode_flag->is_flag_set()){
		follow_mode_flag->reset_flag();
		follow_mode_active = follow_mode;
		freq_update_flag->set_flag();
	}

	// Reinitialize the channel 2 timer if frequency is adjusted in follower mode
	if(freq_update_flag->is_flag_set() && follow_mode_active){
		freq_update_flag->reset_flag();
		follow_mode_freq = follow_f;
		reinitialize_timer(follow_mode_freq, LUT_SIZE);
	}

	// If any change is made, restart the DAC
	if(dac_update_flag->is_flag_set()){
		dac_update_flag->reset_flag();

		HAL_DAC_Stop_DMA(dac_handle, dac_channel);
		HAL_TIM_Base_Start(timer_handle); // Start the timer
		HAL_DAC_Start_DMA(dac_handle, dac_channel, (uint32_t*)wave_LUT, LUT_SIZE, dac_alignment); // Output wave table via DMA for corresponding DAC channel
	}
}


void DACOutputDriver::reinitialize_timer(uint32_t frequency, uint16_t LUT_SIZE){
    timer_handle->Init.Period = static_cast<uint32_t>(SYS_CLOCK_FREQ / (LUT_SIZE * frequency)) - 1;
    HAL_TIM_Base_Init(timer_handle); // Reinitialize the timer with a new period
}


uint32_t DACOutputDriver::get_freq(void){
    return (follow_mode_active) ? follow_mode_freq : static_cast<uint32_t>((curr_freq / (float)FREQ_KNOB_STEPS) * MAX_FREQ);
}

