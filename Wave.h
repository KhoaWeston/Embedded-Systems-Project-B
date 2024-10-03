/*
 * Wave.h
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#ifndef INC_WAVE_H_
#define INC_WAVE_H_

#include "main.h"
#include <math.h>

#define LUT_SIZE       			(128)
#define M_PI					3.14159265

enum wave_type{SINE, SAW, PULSE, TRI};


class Wave{
private:
	uint16_t amplitude;
	uint16_t vert_offset;
	uint16_t hor_offset;
	uint32_t sine_wave_table[LUT_SIZE];
	uint32_t sawtooth_wave_table[LUT_SIZE];
	uint32_t tri_wave_table[LUT_SIZE];
	uint32_t pulse_wave_table[LUT_SIZE];
	uint8_t wave_index;
public:
	Wave(void);
	void build_wave(uint16_t amp, uint16_t offset, wave_type, uint16_t);
	void sine_wave_build(void);
	void sawtooth_wave_build(void);
	void tri_wave_build(void);
	void pulse_wave_build(void);
	uint32_t* get_wave_LUT(void);
};


#endif /* INC_WAVE_H_ */
