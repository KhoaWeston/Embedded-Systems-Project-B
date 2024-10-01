/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(uint32_t amp, uint32_t offset){
	amplitude = amp;
	dc_offset = offset;
}


void Wave::wave_build(void){
	uint8_t i;
	for(i = 0; i < LUT_SIZE; i++){
		sine_wave_table[i] = (dc_offset + amplitude * sin(2 * M_PI * i / LUT_SIZE));
		sawtooth_wave_table[i] = (dc_offset + amplitude * (float)(-1. + (2. * ((double)i / (double)LUT_SIZE))));
		pulse_wave_table[i] = ((i < LUT_SIZE/2)? 2* amplitude : 0);
		tri_wave_table[i] = ((i < LUT_SIZE/2)? (-(2*amplitude)/(LUT_SIZE/2)*i+amplitude+dc_offset) : (int)((float)(2 * amplitude) / (LUT_SIZE / 2) * (float)i - 2*amplitude));
	}
}


uint32_t* Wave::get_sine_wave_LUT(void){
	return sine_wave_table;
}


uint32_t* Wave::get_sawtooth_wave_LUT(void){
	return sawtooth_wave_table;
}


uint32_t* Wave::get_pulse_wave_LUT(void){
	return pulse_wave_table;
}


uint32_t* Wave::get_tri_wave_LUT(void){
	return tri_wave_table;
}

