/*
 * DacOutputDriver.h
 *
 *  Created on: Oct 3, 2024
 *      Author: khoaw
 */

#ifndef INC_DACOUTPUTDRIVER_H_
#define INC_DACOUTPUTDRIVER_H_

#include "main.h"
#include "Queue.h"

#define SYS_CLOCK_FREQ			(80000000)	// 80 MHz system clock frequency
#define TIM_PRESCALER 			(4)


/**
 * @brief Manages dual-channel DAC output.
 *
 * This class interfaces with the DAC and Timer peripherals to output
 * specific waveforms based on lookup tables and frequency instructions from queues.
 */
class DACOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	/**
	 * @brief Handles individual DAC channel output.
	 *
	 * This nested class manages the DAC channel's timer frequency, DMA output, and updates
	 * based on queue instructions for specific channel parameters.
	 */
	class IndDAC{ // @suppress("Miss copy constructor or assignment operator")
	private:
		DAC_HandleTypeDef* dac_handle; 		// DAC handle for the channel
		TIM_HandleTypeDef* tim_handle; 		// Timer handle for the channel
		uint32_t dac_channel; 				// DAC channel identifier
		uint32_t dac_alignment; 			// dAC alignment setting

		uint32_t curr_freq_Hz;				// Actual frequency in Hz
		uint16_t lut_size;					// Size oft he lookup table
		uint32_t* lut_point;				// Pointer to the LUT data

		Queue* wave_queue;					// Queue for receiving frequency change instructions
		LUTQueue *lut_wave_queue;			// Queue for receiving wave data
		EventFlag* freq_update_flag;		// Flag for channel synchronization
	public:
		/**
		 * @brief Initializes the DAC channel with provided settings and queues.
		 * @param x dac Pointer to the DAC handle for the channel.
		 * @param x tim Pointer to the timer handle for the channel.
		 * @param x chan DAC channel identifier.
		 * @param x align DAC alignment setting.
		 * @param x size Size of the lookup table.
		 * @param x w_q Pointer to the queue for frequency instructions.
		 * @param x lut_q Pointer to the queue for LUT data.
		 * @param x flag Pointer to the event flag for frequency updates.
		 * @param x l_num Index to dequeue correct LUT pointer.
		 * @param x f_num Index to dequeue correct frequency value.
		 */
		void initialize(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim, uint32_t chan, uint32_t align,
				uint16_t size, Queue* w_q, LUTQueue* lut_q, EventFlag* flag, uint8_t l_num, uint8_t f_num);

		/**
		 * @brief Reinitializes the timer to update the DAC output frequency.
		 *
		 * Adjusts the timer frequency based on the latest frequency setting.
		 */
		void reinitialize_timer(void);

		/**
		 * @brief Restarts the DAC channel with updated waveform settings.
		 *
		 * Ensures that the DAC DMA outputs are in sync.
		 */
		void restart_DAC(void);

		/**
		 * @brief Updates the frequency of the DAC output based on the latest instructions.
		 * @param x f_num Index to dequeue frequency from correct position.
		 */
		void update_freq(uint8_t f_num);

		/**
		 * @brief Retrieves the current frequency of the DAC output.
		 * @return Current frequency in Hz.
		 */
		uint32_t get_freq(void);
	};

	IndDAC dac_ch1;							// Child classes to handle specific DAC channels
	IndDAC dac_ch2;

	Queue* wave_queue;						// Queue for receiving frequency instructions
	LUTQueue *lut_wave_queue;				// Queue for LUT changes
	EventFlag freq1_update_flag; 			// Flag for frequency updates
	EventFlag freq2_update_flag;
public:
	/**
	 * @brief Constructs a DACOutputDriver object.
	 * @param x dac Pointer to the DAC handle for both channels.
	 * @param x tim1_handle Pointer to the timer handle for channel 1.
	 * @param x tim2_handle Pointer to the timer handle for channel 2.
	 * @param x chan1 DAC channel identifier for channel 1.
	 * @param x chan2 DAC channel identifier for channel 2.
	 * @param x align DAC alignment setting.
	 * @param x w_q Pointer to the queue for frequency instructions.
	 * @param x lut_q Pointer to the queue for lookup table (LUT) changes.
	 * @param x size Size of the lookup table used for waveform generation.
	 */
	DACOutputDriver(DAC_HandleTypeDef* dac, TIM_HandleTypeDef* tim1, TIM_HandleTypeDef* tim2,
			uint32_t chan1, uint32_t chan2, uint32_t align, Queue* w_q, LUTQueue* lut_q, uint16_t size);

	/**
	 * @brief Updates both DAC channels based on the latest instructions and waveform data.
	 */
	void update_DAC(void);
};


#endif /* INC_DACOUTPUTDRIVER_H_ */
