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

#define M_PI					(3.14159265)	// Pi value
#define LUT_SIZE       			(128) 			// Lookup table size (i.e. resolution)
#define AMP_KNOB_STEPS			(10)			// Number of steps for amplitude knob
#define DELAY_KNOB_STEPS		(8) 			// Number of steps for delay knob
#define VERT_OFFSET 			(50)			// Bottom and top offset to avoid clipping
#define DAC_VALUE_MAX			(4096)			// Max DAC value

enum WaveType{NO_WAVE, SINE, SAW, SQUARE, TRI}; // Wave type options

extern const uint32_t* base_wave_tables[4];


class Wave{ // @suppress("Miss copy constructor or assignment operator")
private:
	class IndWave{ // @suppress("Miss copy constructor or assignment operator")
	private:
		Queue* queue; 									// Holds instructions for what parameters to change
		EventFlag* wave_update_flag; 					// Flag for wave updates

		uint8_t curr_amp; 								// Index for current amplitude (1 to AMP_KNOB_STEPS)
		WaveType curr_wave; 							// Index for current wave type
		uint32_t output_wave_LUT[LUT_SIZE];				// Final wave lookup table for DAC output
	public:
		IndWave(void);									// Constructor to initialize attributes and wave tables
		void set_pointers(Queue*, EventFlag*);			// Assigns pointer attributes passed from parent
		void update_wave_params(uint8_t, uint8_t); 		// Update wave parameters from queue
		uint32_t* get_active_wave_LUT(void); 			// Return the current wave LUT
		uint16_t get_amplitude(void);					// Returns curr_amp in mV
		WaveType get_wave_type(void);					// Returns curr_wave
		void write_output_wave(void);					// Write current wave into output LUT
	}wave_ch1, wave_ch2; 								// Child classes for individual waves for channel 1 and 2

	Queue *ID_queue; 									// Queue for instructions from InputDriver
	Queue *dis_queue; 									// Queues for sending updates to the display
	LUTQueue *DAC_queue;								// Queue to send LUT pointers to DAC
	EventFlag wave1_update_flag, wave2_update_flag; 	// Flags for wave update synchronization

	bool is_in_follow_mode; 							// Flag to indicate whether wave 1 echos wave 1
	uint8_t delay; 										// Horizontal offset in 45 degree increments for CH2 in follow mode

	void shift_follow_wave(void);						// Shifts the wave 2 output LUT according to delay
public:
	Wave(Queue*, LUTQueue*, Queue*);					// Constructor initializes attributes and enqueues startup instructions to DAC and display
	void update_waves(void); 							// Updates child waves and notifies DAC and display
};


#endif /* INC_WAVE_H_ */
