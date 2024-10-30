/*
 * DacOutputDriver.h
 *
 *  Created on: Oct 3, 2024
 *      Author: khoaw
 */

#ifndef INC_DACOUTPUTDRIVER_H_
#define INC_DACOUTPUTDRIVER_H_

#include "main.h"
#include "stm32l4xx_ll_tim.h"  // Offers the HAL_LL timer functions
#include "Queue.h"

#define FREQ_KNOB_STEPS			(8)			// Number of steps for frequency knob
#define SYS_CLOCK_FREQ			(80000000)	// 80 MHz system clock frequency
#define MAX_FREQ				(10000)		// Max allowed frequency in Hz


class DACOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	class IndDAC{ // @suppress("Miss copy constructor or assignment operator")
	private:
		DAC_HandleTypeDef* dac_handle; 		// DAC hadle for the channel
		TIM_HandleTypeDef* tim_handle; 		// Timer handle fo the channel
		uint32_t dac_channel; 				// DAC channel identifier
		uint32_t dac_alignment; 			// dAC alignment setting

		uint8_t curr_freq; 					// Current frequency index (1 to FREQFREQ_KNOB_STEPS)
		uint32_t curr_freq_Hz;				// Actual frequency in Hz
		uint16_t lut_size;					// Size oft he lookup table
		uint32_t* lut_point;				// Pointer to the LUT data

		Queue* queue;						// Queue for receiving frequency change instructions
		LUTQueue *wave_queue;				// Queue for receiving wave data
		EventFlag* freq_update_flag;		// Flag for channel synchronization
	public:
		IndDAC(void); 						// Constructor initializes channels and enqueues startup values for display
		void setup(DAC_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint16_t, Queue*, LUTQueue*, EventFlag*, uint8_t);
		void reinitialize_timer(uint32_t); 	// Updates timer frequency
		void restart_DAC(void); 			// Restart the DAC DMA output
		void update_freq(uint8_t); 			// Changes frequency based on instructions from queue
		uint32_t get_freq(void); 			// Returns current frequency in Hz
	}dac_ch1, dac_ch2; 						// Child classes to handle specific DAC channels

	Queue* ID_queue;						// Queue for receiving frequency instructions
	Queue *dis_queue;						// Queue for sending change updates to the display
	LUTQueue *wave_queue;					// Queue for LUT changes
	EventFlag freq1_update_flag, freq2_update_flag;		// Flag for frequency updates

	bool follow_mode_active; 				// Indicates whether channel 2 echos channel 1
public:
	DACOutputDriver(DAC_HandleTypeDef*, TIM_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint32_t, Queue*, LUTQueue*, Queue*, uint16_t);
	void update_DAC(void);					// Main function to update DAC channels
};


#endif /* INC_DACOUTPUTDRIVER_H_ */
