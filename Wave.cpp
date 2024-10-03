/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(void){
	amplitude = 0;
	vert_offset = 0;
	wave_index = 0;
	hor_offset = 0;
}


void Wave::build_wave(uint16_t amp, uint16_t y_off, wave_type type, uint16_t x_off){
	amplitude = amp;
	vert_offset = y_off;
	wave_index = type;
	hor_offset = (x_off/100.)*LUT_SIZE;

	switch(wave_index){
		case 0 : sine_wave_build(); break;
		case 1 : sawtooth_wave_build(); break;
		case 2 : pulse_wave_build(); break;
		case 3 : tri_wave_build(); break;
		default : sine_wave_build(); break;
	}
}


void Wave::sine_wave_build(void){
	for(uint8_t i = hor_offset; i < LUT_SIZE+hor_offset; i++){
		sine_wave_table[i] = (vert_offset + amplitude * sin(2 * M_PI * i / LUT_SIZE));
	}
}


void Wave::sawtooth_wave_build(void){
	for(uint8_t i = hor_offset; i < LUT_SIZE+hor_offset; i++){
		sawtooth_wave_table[i] = (vert_offset + amplitude * (float)(-1. + (2. * ((double)i / (double)LUT_SIZE))));
	}
}


void Wave::pulse_wave_build(void){
	for(uint8_t i = hor_offset; i < LUT_SIZE+hor_offset; i++){
		pulse_wave_table[i] = ((i < LUT_SIZE/2)? 2* amplitude/2+vert_offset : -amplitude/2+vert_offset);
	}
}


void Wave::tri_wave_build(void){
	for(uint8_t i = hor_offset; i < LUT_SIZE+hor_offset; i++){
		tri_wave_table[i] = ((i < LUT_SIZE/2)? (-(2*amplitude)/(LUT_SIZE/2)*i+amplitude+vert_offset) : (int)((float)(2 * amplitude) / (LUT_SIZE / 2) * (float)i - 2*amplitude));
	}
}


uint32_t* Wave::get_wave_LUT(void){
	switch(wave_index){
		case 0 : return sine_wave_table;
		case 1 : return sawtooth_wave_table;
		case 2 : return pulse_wave_table;
		case 3 : return tri_wave_table;
		default : return sine_wave_table;
	}
}

