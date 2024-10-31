/*
 * DacOutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include "DacOutputDriver.h"


DACOutputDriver::DACOutputDriver(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim1, TIM_HandleTypeDef* tim2, // @suppress("Class members should be properly initialized")
		uint32_t chan1, uint32_t chan2, uint32_t align, Queue* q1, LUTQueue* q2, Queue* q3, uint16_t s){
	ID_queue = q1;
	wave_queue = q2;
	dis_queue = q3;

	follow_mode_active = false;

	// Initialize DAC channels with setup parameters
	dac_ch1.setup(dac, tim1, chan1, align, s, q1, q2, &freq1_update_flag, LUT1);
	dac_ch2.setup(dac, tim2, chan2, align, s, q1, q2, &freq2_update_flag, LUT2);

	// Initialize display with starting frequencies
	dis_queue->enqueue(FREQ1_1, dac_ch1.get_freq());
	dis_queue->enqueue(FREQ2_1, dac_ch2.get_freq());
}


// Set the output value of the DAC via DMA
void DACOutputDriver::update_DAC(void){
	// Update frequency of each DAC channel
	dac_ch1.update_freq(FREQ1_0);
	dac_ch2.update_freq(FREQ2_0);

	uint32_t follow_mode_change;

	// Handle channels if follow mode is toggled
	if(ID_queue->dequeue(FOLLOW_1, follow_mode_change)){
		follow_mode_active = (follow_mode_change == MODE_ON);
		freq2_update_flag.set_flag();
	}

	// If channels are changed, sync DACs and update displays with new frequencies
	if(freq1_update_flag.is_flag_set()){
		freq1_update_flag.reset_flag();

		// Reinitialize channel 2 timer if frequency channel 1 is adjusted in follower mode
		if(follow_mode_active){
			freq2_update_flag.set_flag();
		}else{
			dac_ch1.restart_DAC();
			dac_ch2.restart_DAC();
		}

		dis_queue->enqueue(FREQ1_1, dac_ch1.get_freq());
	}

	if(freq2_update_flag.is_flag_set()){
		freq2_update_flag.reset_flag();

		if(follow_mode_active){
			dac_ch2.reinitialize_timer(dac_ch1.get_freq_idx());
		}

		dac_ch1.restart_DAC();
		dac_ch2.restart_DAC();

		dis_queue->enqueue(FREQ2_1, dac_ch2.get_freq());
	}
}


DACOutputDriver::IndDAC::IndDAC(){ // @suppress("Class members should be properly initialized")
	curr_freq = 1;
}


void DACOutputDriver::IndDAC::setup(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim, uint32_t chan, uint32_t align, uint16_t s, Queue* q, LUTQueue* q2, EventFlag* f, uint8_t l_num){
	dac_handle = dac;
	tim_handle = tim;
	dac_channel = chan;
	dac_alignment = align;

	lut_size = s;

	queue = q;
	wave_queue = q2;
	freq_update_flag = f;

	wave_queue->dequeue(l_num, lut_point);		// Load the waveform data

	reinitialize_timer(curr_freq);			// Set the initial frequency
	HAL_TIM_Base_Start(tim_handle); 			// Start the timer
	restart_DAC();
}


void DACOutputDriver::IndDAC::update_freq(uint8_t f_num){
	uint32_t freq_change;

	// Adjust the timer frequency
	if(queue->dequeue(f_num, freq_change)){
		if(freq_change == INC){
			curr_freq = (curr_freq == FREQ_KNOB_STEPS) ? 1 : curr_freq+1;
		}else if(freq_change == DEC){
			curr_freq = (curr_freq == 1) ? FREQ_KNOB_STEPS : curr_freq-1;
		}

		freq_update_flag->set_flag();
		reinitialize_timer(curr_freq);
	}
}


void DACOutputDriver::IndDAC::restart_DAC(void){
	HAL_DAC_Stop_DMA(dac_handle, dac_channel);
	HAL_DAC_Start_DMA(dac_handle, dac_channel, lut_point, lut_size, dac_alignment);
}

void DACOutputDriver::IndDAC::reinitialize_timer(uint8_t freq_idx){
	curr_freq_Hz = (freq_idx == 1) ? 1 : static_cast<uint32_t>(((float)freq_idx / FREQ_KNOB_STEPS) * MAX_FREQ);
    uint32_t tim_period = SYS_CLOCK_FREQ / (lut_size * curr_freq_Hz) - 1;
    tim_handle->Instance->ARR = tim_period;
    tim_handle->Init.Period = tim_period;
}


uint8_t DACOutputDriver::IndDAC::get_freq_idx(void){
	return curr_freq;
}


uint32_t DACOutputDriver::IndDAC::get_freq(void){
    return curr_freq_Hz;
}

