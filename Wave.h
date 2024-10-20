/*
 * Wave.h
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#ifndef INC_WAVE_H_
#define INC_WAVE_H_

#include "main.h"
#include "Queue.h"
#include <math.h>

#define LUT_SIZE       			(256) 	// Lookup table size (i.e. resolution)
#define M_PI					3.14159265
#define AMP_KNOB_STEPS			(8)		// Number of steps for amplitude knob

enum WaveType{SINE, SAW, SQUARE, TRI}; // Wave type options


class Wave{ // @suppress("Miss copy constructor or assignment operator")
private:
	uint8_t curr_amp;
	uint8_t delay; 								// Horizontal offset in 45 degree increments for CH2
	WaveType curr_wave;

	WaveQueue* queue;							// Instructions for parameter change
	Wave* follower_wave;						// Reference to the other wave (for follower mode)
	EventFlag* follow_mode_flag; 				// Flag for follow mode activation
	EventFlag* wave_update_flag; 				// Flag for wave updates

	uint32_t base_wave_tables[4][LUT_SIZE];		// General wave lookup tables (SINE, SAW, SQUARE, TRI)
	uint32_t scaled_wave_tables[4][LUT_SIZE];	// Scaled wave lookup tables (SINE, SAW, SQUARE, TRI)

	bool is_in_follow_mode;
	uint32_t follow_wave_LUT[LUT_SIZE];			// Holds the adjusted wave when in follow mode
	uint16_t follow_mode_amp; 					// Amplitude when in follow mode
	WaveType follow_mode_wave;					// Holds the type of the wave in follow mode
public:
	Wave(Wave*, WaveType, uint8_t, WaveQueue*, EventFlag*, EventFlag*);
	void generate_waves(void); 				// Build all general and scaled waveforms
	void update_wave_params(void); 			// Update wave parameters from queue
	uint32_t* get_active_wave_LUT(void); 	// Return the current wave lookup table
	void scale_waves(void);					// Scale all general waves based on amplitude
	uint16_t get_amplitude(void);
	uint16_t get_delay(void);
	WaveType get_wave_type(void);
	bool is_follow_mode_active(void);
};


#endif /* INC_WAVE_H_ */
