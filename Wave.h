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
#include "Queue.h"
#include <algorithm>

#define LUT_SIZE       			(128) 		// Lookup table size (i.e. resolution)
#define M_PI					3.14159265
#define A_KNOB_INTERVALS		(8)

enum wave_type{SINE, SAW, SQUARE, TRI}; // wave type options


class Wave{ // @suppress("Miss copy constructor or assignment operator")
private:
	uint8_t amplitude; 						// Holds the user-specified amplitude
	uint8_t hor_offset; 						// Holds the horizontal offset in 45 degree increments for CH2
	uint8_t wave_index; 						// Holds the user-specified wave type
	Queue* queue;								// Holds the instructions for what parameters to change
	Wave* wave;
	uint32_t sine_wave_table[LUT_SIZE]; 		// Holds the sine wave table
	uint32_t sawtooth_wave_table[LUT_SIZE]; 	// Holds the sawtooth wave table
	uint32_t tri_wave_table[LUT_SIZE]; 			// Holds the triangle wave table
	uint32_t square_wave_table[LUT_SIZE]; 		// Holds the pulse wave table
	uint32_t scaled_sine_wave_table[LUT_SIZE]; 		// Holds the sine wave table
	uint32_t scaled_sawtooth_wave_table[LUT_SIZE]; 	// Holds the sawtooth wave table
	uint32_t scaled_tri_wave_table[LUT_SIZE]; 			// Holds the triangle wave table
	uint32_t scaled_square_wave_table[LUT_SIZE]; 		// Holds the pulse wave table

	bool follow_mode;
	uint32_t follow_wave_LUT[LUT_SIZE];
	uint16_t follow_amp;
	uint8_t follow_wave_ind;
public:
	Wave(Wave*, wave_type, uint8_t, Queue*); // Initializes the Wave class attributes
	void build_waves(void);
	void update_wave(); // Builds specific wave table depending on user-specified wave type
	uint32_t* get_wave_LUT(void); // Returns the lookup table for the user-specified wave type
	void scale_all(void);
	uint16_t get_amp(void);
	uint16_t get_hor_off(void);
	uint8_t get_type(void);
	bool get_follow_mode(void);
};


#endif /* INC_WAVE_H_ */
