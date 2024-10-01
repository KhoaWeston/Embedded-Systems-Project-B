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


class Wave{
private:
	uint32_t amplitude;
	uint32_t dc_offset;
	uint32_t sine_wave_table[LUT_SIZE];
	uint32_t sawtooth_wave_table[LUT_SIZE];
	uint32_t tri_wave_table[LUT_SIZE];
	uint32_t pulse_wave_table[LUT_SIZE];
public:
	Wave(uint32_t amp, uint32_t offset);
	void wave_build(void);
	uint32_t* get_sine_wave_LUT(void);
	uint32_t* get_sawtooth_wave_LUT(void);
	uint32_t* get_pulse_wave_LUT(void);
	uint32_t* get_tri_wave_LUT(void);
};


#endif /* INC_WAVE_H_ */
