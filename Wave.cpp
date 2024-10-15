/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(Wave* w, wave_type type, uint8_t amp, Queue* q){ // Initializes the Wave class attributes
	wave_index = type;
	amplitude = amp;
	hor_offset = 0;
	queue = q;
	wave = w;

	follow_mode = false;
	follow_amp = 1;
	follow_wave_ind = 0;
}


void Wave::build_waves(void){
	uint16_t max_amp = 2048;
	uint16_t vert_offset = 50;

	for(uint16_t i = 0; i < LUT_SIZE; i++){
		sine_wave_table[i] = max_amp * (sin(2 * M_PI * i / LUT_SIZE) + 1) + vert_offset; // Build sine
		sawtooth_wave_table[i] = 2 * max_amp * (float)((float)i / (LUT_SIZE - 1)) + vert_offset; // Build sawtooth
		square_wave_table[i] = (i < LUT_SIZE / 2) ? 2 * max_amp + vert_offset : vert_offset; // Build square
		tri_wave_table[i] = abs((2 * max_amp) * (-(float)i / (LUT_SIZE / 2) + 1)) + vert_offset; // Build triangle
	}

	scale_all();
}


void Wave::update_wave(void){
	uint8_t wave_move = 0;
	uint8_t amp_move = 0;
	uint8_t off_move = 0;
	uint8_t follow_move = 0;

	bool wave_flag = queue->dequeue(&wave_move, WAVE);
	bool amp_flag = queue->dequeue(&amp_move, AMP);
	bool off_flag = queue->dequeue(&off_move, OFF);
	bool follow_flag = queue->dequeue(&follow_move, FOLLOW);

	if(follow_flag){
		if(follow_move == INC){
			if(follow_mode){
				follow_mode = false;
			}else{
				follow_mode = true;

				uint32_t* copy_LUT = wave->get_wave_LUT(); // Copy wave1 table
				for(uint16_t i = 0; i < LUT_SIZE; i++){
					follow_wave_LUT[i] = copy_LUT[i];
				}
			}
		}
	}

	if(follow_mode){
		follow_amp = wave->get_amp();
		follow_wave_ind = wave->get_type();
	}

	if(wave_flag){
		if(wave_move == INC){
			if(wave_index == 3){
				wave_index = 0;
			}else{
				wave_index++;
			}
		}
	}

	if(amp_flag){
		if(amp_move == INC){
			if(amplitude >= A_KNOB_INTERVALS){
				amplitude = 1;
			}else{
				amplitude++;
			}
		}else if(amp_move == DEC){
			if(amplitude <= 1){
				amplitude = A_KNOB_INTERVALS;
			}else{
				amplitude--;
			}
		}
		scale_all();
	}

	if(off_flag){
		if(off_move == INC){
			if(hor_offset == A_KNOB_INTERVALS-1){
				hor_offset = 0;
			}else{
				hor_offset++;
			}
		}else if(off_move == DEC){
			if(hor_offset == 0){
				hor_offset = A_KNOB_INTERVALS-1;
			}else{
				hor_offset--;
			}
		}

		uint32_t temp[LUT_SIZE];
		uint16_t x_off = (hor_offset/8.)*LUT_SIZE;
		for (uint16_t i = 0; i < LUT_SIZE; i++) {
			temp[i] = follow_wave_LUT[(i + x_off) % LUT_SIZE];
		}
		for (uint16_t i = 0; i < LUT_SIZE; i++) {
			follow_wave_LUT[i] = temp[i];
		}
	}
}


uint32_t* Wave::get_wave_LUT(void){ // Returns the lookup table for the user-specified wave type
	if(follow_mode){
		return follow_wave_LUT;
	}else{
		switch(wave_index){
			case 0 : return scaled_sine_wave_table;
			case 1 : return scaled_sawtooth_wave_table;
			case 2 : return scaled_square_wave_table;
			case 3 : return scaled_tri_wave_table;
			default : return scaled_sine_wave_table;
		}
	}
}


void Wave::scale_all(void){
	float scale_factor = amplitude/(float)A_KNOB_INTERVALS;
	for(uint16_t i = 0; i < LUT_SIZE; i++){
		scaled_sine_wave_table[i] = sine_wave_table[i] * scale_factor;
		scaled_sawtooth_wave_table[i] = sawtooth_wave_table[i] * scale_factor;
		scaled_square_wave_table[i] = square_wave_table[i] * scale_factor;
		scaled_tri_wave_table[i] = tri_wave_table[i] * scale_factor;
	}
}


uint16_t Wave::get_amp(void){
	if(follow_mode){
		return follow_amp;
	}else{
		return (int)((amplitude/(float)A_KNOB_INTERVALS)*2048);
	}
}


uint16_t Wave::get_hor_off(void){
	return (int)(hor_offset/(float)A_KNOB_INTERVALS*100);
}


uint8_t Wave::get_type(void){
	if(follow_mode){
		return follow_wave_ind;
	}else{
		return wave_index;
	}
}


bool Wave::get_follow_mode(void){
	return follow_mode;
}
