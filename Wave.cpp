/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(Wave* w, WaveType type, uint8_t amp, WaveQueue* q, EventFlag* f1, EventFlag* f2){ // @suppress("Class members should be properly initialized")
	curr_wave = type;
	curr_amp = amp;
	delay = 0;

	queue = q;
	follow_mode_flag = f1;
	wave_update_flag = f2;

	follower_wave = w;
	is_in_follow_mode = false;
	follow_mode_amp = 1;
	follow_mode_wave = SINE;
}


void Wave::generate_waves(void){ // Build all general and scaled waveforms
	const uint16_t vert_offset = 75;
	const uint16_t max_amp = 2048-vert_offset;

	for(uint16_t i = 0; i < LUT_SIZE; i++){
		base_wave_tables[SINE][i] = max_amp * (sin(2 * M_PI * i / LUT_SIZE) + 1) + vert_offset; 			// Build sine
		base_wave_tables[SAW][i] = 2 * max_amp * (float)i / (LUT_SIZE - 1) + vert_offset; 					// Build sawtooth
		base_wave_tables[SQUARE][i] = (i < LUT_SIZE / 2) ? 2 * max_amp + vert_offset : vert_offset; 		// Build square
		base_wave_tables[TRI][i] = abs((2 * max_amp) * (-(float)i / (LUT_SIZE / 2) + 1)) + vert_offset; 	// Build triangle
	}

	scale_waves();
}


void Wave::update_wave_params(void){ // Update wave parameters from queue
	uint8_t wave_change = 0;
	uint8_t amp_change = 0;
	uint8_t follow_mode_change = 0;
	uint8_t delay_change = 0;

	bool waveform_updated  = queue->dequeue(&wave_change, WAVE);
	bool amp_updated = queue->dequeue(&amp_change, AMP);
	bool follow_mode_updated = queue->dequeue(&follow_mode_change, FOLLOW);
	bool delay_updated = queue->dequeue(&delay_change, DEL);

	// Cycle through wave types
	if(waveform_updated && wave_change == INC){
		curr_wave = static_cast<WaveType>((curr_wave + 1) % 4);
		wave_update_flag->set_flag();
	}

	// Change amplitude of all wave tables
	if(amp_updated){
		if(amp_change == INC){
			curr_amp = (curr_amp >= AMP_KNOB_STEPS) ? 1 : curr_amp+1;
		}else if(amp_change == DEC){
			curr_amp = (curr_amp <= 1) ? AMP_KNOB_STEPS : curr_amp-1;
		}
		wave_update_flag->set_flag();
		scale_waves();
	}

	// Toggle follow mode
	if(follow_mode_updated){
		is_in_follow_mode = (follow_mode_change == INC) ? !is_in_follow_mode : is_in_follow_mode;
		follow_mode_flag->set_flag();
		wave_update_flag->set_flag();
	}

	// While in follow mode, if wave 1 changes, then change wave 2
	if(wave_update_flag->is_flag_set() && is_in_follow_mode){
		wave_update_flag->reset_flag();
		follow_mode_amp = follower_wave->get_amplitude();
		follow_mode_wave = follower_wave->get_wave_type();

		uint32_t* copy_LUT = follower_wave->get_active_wave_LUT();
		for(uint16_t i = 0; i < LUT_SIZE; i++){
			follow_wave_LUT[i] = copy_LUT[i];
		}
	}

	// Adjust delay of wave 2 while in follow mode
	if(delay_updated){
		if(delay_change == INC){
			delay = (delay == AMP_KNOB_STEPS-1) ? 0 : delay+1;
		}else if(delay_change == DEC){
			delay = (delay == 0) ? AMP_KNOB_STEPS-1 : delay-1;
		}

		uint32_t temp[LUT_SIZE];
		uint16_t delay_index = (delay/8.)*LUT_SIZE;
		for (uint16_t i = 0; i < LUT_SIZE; i++){ // Make a copy of table with delay adjustment
			temp[i] = follow_wave_LUT[(i + delay_index) % LUT_SIZE];
		}
		for (uint16_t i = 0; i < LUT_SIZE; i++){ // Copy back into original table
			follow_wave_LUT[i] = temp[i];
		}
	}
}


void Wave::scale_waves(void){ // Scale all general waves based on amplitude
	float scale_factor = curr_amp/(float)AMP_KNOB_STEPS;
	for(uint16_t i = 0; i < LUT_SIZE; i++){
		scaled_wave_tables[SINE][i] = base_wave_tables[SINE][i] * scale_factor;
		scaled_wave_tables[SAW][i] = base_wave_tables[SAW][i] * scale_factor;
		scaled_wave_tables[SQUARE][i] = base_wave_tables[SQUARE][i] * scale_factor;
		scaled_wave_tables[TRI][i] = base_wave_tables[TRI][i] * scale_factor;
	}
}


uint32_t* Wave::get_active_wave_LUT(void){ // Return the current wave lookup table
	return (is_in_follow_mode) ? follow_wave_LUT : scaled_wave_tables[curr_wave];
}


uint16_t Wave::get_amplitude(void) {
    return (is_in_follow_mode) ? follow_mode_amp : static_cast<uint16_t>((curr_amp / (float)AMP_KNOB_STEPS) * 2048);
}


uint16_t Wave::get_delay(void) {
    return static_cast<uint16_t>((delay / (float)AMP_KNOB_STEPS) * 100);
}


WaveType Wave::get_wave_type(void) {
    return (is_in_follow_mode) ? follow_mode_wave : curr_wave;
}


bool Wave::is_follow_mode_active(void){
	return is_in_follow_mode;
}

