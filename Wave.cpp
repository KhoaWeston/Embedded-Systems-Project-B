/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(Queue* q, LUTQueue* q2, Queue* q3){ // @suppress("Class members should be properly initialized")
	ID_queue = q;
	DAC_queue = q2;
	dis_queue = q3;

	is_in_follow_mode = false;
	delay = DELAY_KNOB_STEPS;

	// Set external dependencies for each wave child
	wave_ch1.set_pointers(ID_queue, &wave1_update_flag);
	wave_ch2.set_pointers(ID_queue, &wave2_update_flag);

	// Initialize DAC and display with initial waveforms and parameters
	DAC_queue->enqueue(LUT1, (uint32_t*)wave_ch1.get_active_wave_LUT());
	DAC_queue->enqueue(LUT2, (uint32_t*)wave_ch2.get_active_wave_LUT());

	dis_queue->enqueue(AMP1_1, wave_ch1.get_amplitude());
	dis_queue->enqueue(TYPE1_1, wave_ch1.get_wave_type());
	dis_queue->enqueue(AMP2_1, wave_ch2.get_amplitude());
	dis_queue->enqueue(TYPE2_1, wave_ch2.get_wave_type());
}


void Wave::update_waves(void){
	// Update wave parameters for each child
	wave_ch1.update_wave_params(TYPE1_0, AMP1_0);
	wave_ch2.update_wave_params(TYPE2_0, AMP2_0);

	uint32_t follow_mode_change, delay_change;

	// Toggle follow mode
	if(ID_queue->dequeue(FOLLOW_0, follow_mode_change)){
		//assert(!(follow_mode_change == true || follow_mode_change == false));
		is_in_follow_mode = (follow_mode_change == MODE_ON);
		wave2_update_flag.set_flag();

		if(is_in_follow_mode){
			shift_follow_wave();
		}else{
			wave_ch2.write_output_wave();
		}
	}

	// Synchronize Channel 2 with Channel 1 if in follow mode
	if(wave1_update_flag.is_flag_set() && is_in_follow_mode){
		wave2_update_flag.set_flag();
		shift_follow_wave();
	}

	// Adjust delay of wave 2 while in follow mode
	if(ID_queue->dequeue(DEL_0, delay_change)){
		//assert(!(delay_change == INC || delay_change == DEC));
		if(delay_change == INC){
			delay = (delay == DELAY_KNOB_STEPS) ? 1 : delay+1;
		}else if(delay_change == DEC){
			delay = (delay == 1) ? DELAY_KNOB_STEPS : delay-1;
		}
		wave2_update_flag.set_flag();
		shift_follow_wave();

		uint16_t delay_in_percent = static_cast<uint16_t>((delay / (float)AMP_KNOB_STEPS) * 100);
		dis_queue->enqueue(DEL_1, delay_in_percent);
	}

	// Update display if wave settings have changed
	if(wave1_update_flag.is_flag_set()){
		wave1_update_flag.reset_flag();
		dis_queue->enqueue(AMP1_1, wave_ch1.get_amplitude());
		dis_queue->enqueue(TYPE1_1, wave_ch1.get_wave_type());
	}
	if(wave2_update_flag.is_flag_set()){
		wave2_update_flag.reset_flag();
		if(is_in_follow_mode){
			dis_queue->enqueue(AMP2_1, wave_ch1.get_amplitude());
			dis_queue->enqueue(TYPE2_1, wave_ch1.get_wave_type());
		}else{
			dis_queue->enqueue(AMP2_1, wave_ch2.get_amplitude());
			dis_queue->enqueue(TYPE2_1, wave_ch2.get_wave_type());
		}
	}
}


void Wave::shift_follow_wave(void){
	uint32_t* base_LUT = wave_ch1.get_active_wave_LUT();
	uint32_t* shifted_LUT = wave_ch2.get_active_wave_LUT();
	uint16_t shift_idx = (delay / (float)DELAY_KNOB_STEPS) * LUT_SIZE;

	for (uint16_t i = 0; i < LUT_SIZE; i++){
		shifted_LUT[i] = base_LUT[(i + shift_idx) % LUT_SIZE];
	}
}


Wave::IndWave::IndWave(){
	curr_wave = SINE;
	curr_amp = 1;

	generate_waves();
	scale_waves();
	write_output_wave();
}


void Wave::IndWave::write_output_wave(void){
	for (uint16_t i = 0; i < LUT_SIZE; i++){
		output_wave_LUT[i] = scaled_wave_tables[curr_wave-1][i];
	}
}


void Wave::IndWave::generate_waves(void){ // Build all general and scaled waveforms
	for(uint16_t i = 0; i < LUT_SIZE; i++){
		base_wave_tables[0][i] = (DAC_VALUE_MAX / 2 - VERT_OFFSET) * (sin(2 * M_PI * i / LUT_SIZE) + 1); 			// Build sine
		base_wave_tables[1][i] = 2 * (DAC_VALUE_MAX / 2 - VERT_OFFSET) * (float)i / (LUT_SIZE - 1); 				// Build sawtooth
		base_wave_tables[2][i] = (i < LUT_SIZE / 2) ? 2 * (DAC_VALUE_MAX / 2 - VERT_OFFSET) : 0; 					// Build square
		base_wave_tables[3][i] = abs((2 * (DAC_VALUE_MAX / 2 - VERT_OFFSET)) * (-(float)i / (LUT_SIZE / 2) + 1)) ; 	// Build triangle
	}
}


void Wave::IndWave::scale_waves(void){ // Scale all general waves based on amplitude
	float scale_factor = curr_amp / (float)AMP_KNOB_STEPS;

	for(uint16_t i = 0; i < LUT_SIZE; i++){
		for(uint8_t j = 0; j < 4; j++){
			scaled_wave_tables[j][i] = base_wave_tables[j][i] * scale_factor + VERT_OFFSET;
		}
	}
}


void Wave::IndWave::set_pointers(Queue *q, EventFlag* f){
	queue = q;
	wave_update_flag = f;
}


void Wave::IndWave::update_wave_params(uint8_t w_num, uint8_t a_num){ // Update wave parameters from queue
	uint32_t wave_change, amp_change;

	// Cycle through wave types
	bool waveform_updated = queue->dequeue(w_num, wave_change);
	if(waveform_updated && wave_change == INC){
		//assert(!(wave_change == INC));
		curr_wave = static_cast<WaveType>((curr_wave % 4) + 1);
		write_output_wave();
		wave_update_flag->set_flag();
	}

	// Change amplitude of all wave tables
	if(queue->dequeue(a_num, amp_change)){
		//assert(!(amp_change == INC || amp_change == DEC));

		if(amp_change == INC){
			curr_amp = (curr_amp == AMP_KNOB_STEPS) ? 1 : curr_amp+1;
		}else if(amp_change == DEC){
			curr_amp = (curr_amp == 1) ? AMP_KNOB_STEPS : curr_amp-1;
		}
		scale_waves();
		write_output_wave();
		wave_update_flag->set_flag();
	}
}


uint32_t* Wave::IndWave::get_active_wave_LUT(void){ // Return the current wave lookup table
	return output_wave_LUT;
}


uint16_t Wave::IndWave::get_amplitude(void) {
	const uint16_t max_dac_out = 3300; // in mV
    return static_cast<uint16_t>((curr_amp / (float)AMP_KNOB_STEPS) * max_dac_out);
}


WaveType Wave::IndWave::get_wave_type(void) {
    return curr_wave;
}

