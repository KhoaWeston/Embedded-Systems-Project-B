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

#define LUT_SIZE       			(128) 		// Lookup table size (i.e. resolution)
#define M_PI					3.14159265

enum wave_type{SINE, SAW, PULSE, TRI}; // wave type options


class Wave{
private:
	uint16_t amplitude; 						// Holds the user-specified amplitude
	uint16_t hor_offset; 						// Holds the user-specified horizontal offset
	uint32_t sine_wave_table[LUT_SIZE]; 		// Holds the sine wave table
	uint32_t sawtooth_wave_table[LUT_SIZE]; 	// Holds the sawtooth wave table
	uint32_t tri_wave_table[LUT_SIZE]; 			// Holds the triangle wave table
	uint32_t pulse_wave_table[LUT_SIZE]; 		// Holds the pulse wave table
	uint8_t wave_index; 						// Holds the user-specified wave type
public:
	Wave(wave_type, uint16_t, uint16_t); // Initializes the Wave class attributes
	void update_wave(void); // Builds specific wave table depending on user-specified wave type
	void sine_wave_build(void);
	void sawtooth_wave_build(void);
	void tri_wave_build(void);
	void pulse_wave_build(void);
	uint32_t* get_wave_LUT(void); // Returns the lookup table for the user-specified wave type
	uint16_t get_amp(void);
	uint16_t get_hor_off(void);
	uint8_t get_type(void);
};


#endif /* INC_WAVE_H_ */
