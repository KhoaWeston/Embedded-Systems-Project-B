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

#define LUT_SIZE       			(256) 			// Lookup table size (i.e. resolution)
#define DAC_VALUE_MAX			(4096)			// Max DAC value
#define VERT_OFFSET 			(25)			// Bottom and top offset to avoid clipping
#define MAX_FREQ				(1000)			// Max allowed frequency in Hz
#define NUM_WAVE_TYPES 			(3)
#define AMP_KNOB_STEPS			(10)			// Number of steps for amplitude knob
#define FREQ_KNOB_STEPS			(10)			// Number of steps for frequency knob
#define DELAY_KNOB_STEPS		(8) 			// Number of steps for delay knob
#define M_PI					(3.14159265)	// Pi value

enum WaveType{NO_WAVE, SINE, SQUARE, PULSE}; // Wave type options

extern const uint32_t* base_wave_tables[4];


/**
 * @brief Manages wave parameters and lookup tables (LUT) for 2 channels.
 *
 * This class uses queues and event flags to synchronize wave generation
 * parameters such as frequency, amplitude, waveform type, and delay for two channels.
 */
class Wave{ // @suppress("Miss copy constructor or assignment operator")
private:
	/**
	 * @brief Handles individual wave parameters and lookup table for one channel.
	 *
	 * This nested class manages wave attributes such as frequency, amplitude, and waveform
	 * type, and prepares a final LUT for output based on these parameters.
	 */
	class IndWave{ // @suppress("Miss copy constructor or assignment operator")
	private:
		Queue* ID_queue; 								// Holds instructions for what parameters to change
		EventFlag* wave_update_flag; 					// Flag for wave updates
		EventFlag* freq_update_flag;					// Flag for channel synchronization

		uint8_t curr_amp; 								// Index for current amplitude (1 to AMP_KNOB_STEPS)
		WaveType curr_wave; 							// Index for current wave type
		uint8_t curr_freq; 								// Current frequency index (1 to FREQFREQ_KNOB_STEPS)
		uint32_t output_wave_LUT[LUT_SIZE];				// Final wave lookup table for DAC output
	public:
		/**
		 * @brief Constructs an IndWave object and initializes wave attributes and LUT.
		 */
		IndWave(void);

		/**
		 * @brief Initializes the IndWave object with pointers from the parent Wave class.
		 * @param x id_q Pointer to the queue for ID instructions.
		 * @param x w_flag Pointer to the event flag for wave updates.
		 * @param x f_flag Pointer to the event flag for frequency synchronization.
		 */
		void initialize(Queue *id_q, EventFlag* w_flag, EventFlag* f_flag);

		/**
		 * @brief Updates the wave parameters (wave type, amplitude, frequency) based on queue input.
		 * @param x w_num Wave type index for dequeue.
		 * @param x a_num Amplitude index for dequeue.
		 * @param x f_num Frequency index for dequeue.
		 */
		void update_wave_params(uint8_t w_num, uint8_t a_num, uint8_t f_num);

		/**
		 * @brief Retrieves the current wave lookup table.
		 * @return Pointer to the active LUT for DAC output.
		 */
		uint32_t* get_active_wave_LUT(void);


		/**
		 * @brief Returns the current amplitude setting.
		 * @return Amplitude in mV.
		 */
		uint16_t get_amplitude(void);

		/**
		 * @brief Returns the current wave type.
		 * @return Current wave type as a WaveType enum value.
		 */
		WaveType get_wave_type(void);

		/**
		 * @brief Returns the current frequency in Hz.
		 * @return Frequency in Hz.
		 */
		uint32_t get_freq_Hz(void);

		/**
		 * @brief Writes the configured waveform into the output LUT.
		 *
		 * Generates the waveform based on the current parameters and stores it
		 * in the LUT for DAC output.
		 */
		void write_output_wave(void);
	};

	IndWave wave_ch1;									// Child classes for individual waves for channel 1 and 2
	IndWave wave_ch2;

	Queue *ID_queue; 									// Queue for instructions from InputDriver
	Queue *dis_queue; 									// Queues for sending updates to the display
	Queue *DAC_queue;
	LUTQueue *LUT_DAC_queue;							// Queue to send LUT pointers to DAC
	EventFlag wave1_update_flag;  						// Flags for wave update synchronization
	EventFlag wave2_update_flag;
	EventFlag freq1_update_flag; 						// Flag for frequency updates
	EventFlag freq2_update_flag;

	bool is_in_follow_mode; 							// Flag to indicate whether wave 1 echos wave 1
	uint8_t delay; 										// Horizontal offset in 45 degree increments for CH2 in follow mode

	/**
	 * @brief Shifts the LUT for channel 2 in follow mode based on the set delay.
	 *
	 * Modifies the output LUT for wave 2 to apply a phase shift according to the
	 * specified delay in 45-degree increments.
	 */
	void shift_follow_wave(void);

	/**
	 * @brief Initializes the parent Wave class settings and flags.
	 *
	 * Configures initial wave parameters, updates flags, and prepares
	 * queues for synchronization with DAC and display components.
	 */
	void initialize_parent(void);
public:
	/**
	 * @brief Constructs a Wave object, initializes attributes, and enqueues startup instructions.
	 * @param x id_q Pointer to the queue for input instructions from the InputDriver.
	 * @param x dac_q Pointer to the queue for DAC frequency instructions.
	 * @param x lut_q Pointer to the queue for LUT data sent to DAC.
	 * @param x dis_q Pointer to the queue for display updates.
	 */
	Wave(Queue* id_q, Queue* dac_q, LUTQueue* lut_q, Queue* dis_q);

	/**
	 * @brief Updates both wave channels and notifies the DAC and display.
	 *
	 * Fetches parameters from queues to modify wave parameters for each channel
	 * and manages synchronization flags as necessary.
	 */
	void update_waves(void);
};


#endif /* INC_WAVE_H_ */
