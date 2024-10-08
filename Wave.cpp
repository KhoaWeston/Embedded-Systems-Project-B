/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(wave_type type, uint8_t amp, uint8_t x_off /*, Queue* q*/){ // Initializes the Wave class attributes
	wave_index = type;
	amplitude = (amp/8.)*2048;
	hor_offset = (x_off/8.)*LUT_SIZE;
	/*queue = q;*/
}


void Wave::update_wave(void){
	// TODO: Dequeue the queue to make the appropriate changes to the attributes

	switch(wave_index){ // Builds specific wave table depending on user-specified wave type
		case 0 : sine_wave_build(); break;
		case 1 : sawtooth_wave_build(); break;
		case 2 : square_wave_build(); break;
		case 3 : tri_wave_build(); break;
		default : sine_wave_build(); break;
	}
}


void Wave::sine_wave_build(void){
	for(uint8_t i = 0; i < LUT_SIZE; i++){
		sine_wave_table[i] = amplitude * (sin(2 * M_PI * (i+hor_offset) / LUT_SIZE) + 1) + 50;
	}
}


void Wave::sawtooth_wave_build(void){
	for(uint8_t i = 0; i < LUT_SIZE; i++){
		uint16_t idx = (i + hor_offset < LUT_SIZE) ? i + hor_offset : i - LUT_SIZE + hor_offset;
		sawtooth_wave_table[i] = 2 * amplitude * (float)((float)idx / (LUT_SIZE - 1)) + 50;	}
}


void Wave::square_wave_build(void){
	for(uint8_t i = 0; i < LUT_SIZE; i++){
		if(i < LUT_SIZE / 2 - hor_offset || i + hor_offset >= LUT_SIZE) {
			pulse_wave_table[i] = 2 * amplitude + 50;
		} else {
			pulse_wave_table[i] = 50;
		}
	}
}


void Wave::tri_wave_build(void){
	for(uint8_t i = 0; i < LUT_SIZE; i++){
		uint16_t idx = (i + hor_offset < LUT_SIZE) ? i + hor_offset : i - LUT_SIZE + hor_offset;
		tri_wave_table[i] = abs((2 * amplitude) * (-(float)idx / (LUT_SIZE / 2) + 1)) + 50;	}
}


uint32_t* Wave::get_wave_LUT(void){ // Returns the lookup table for the user-specified wave type
	switch(wave_index){
		case 0 : return sine_wave_table;
		case 1 : return sawtooth_wave_table;
		case 2 : return pulse_wave_table;
		case 3 : return tri_wave_table;
		default : return sine_wave_table;
	}
}


uint16_t Wave::get_amp(void){
	return amplitude;
}


uint16_t Wave::get_hor_off(void){
	return hor_offset;
}


uint8_t Wave::get_type(void){
	return wave_index;
}

