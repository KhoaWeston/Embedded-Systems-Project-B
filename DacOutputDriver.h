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
#define TIM_PRESCALER 			(10)


class DACOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
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
		void initialize(DAC_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint16_t, Queue*, LUTQueue*, EventFlag*, uint8_t, uint8_t);
		void reinitialize_timer(void); 		// Updates timer frequency
		void restart_DAC(void); 			// Restart the DAC DMA output
		void update_freq(uint8_t); 			// Changes frequency based on instructions from queue
	}dac_ch1, dac_ch2; 						// Child classes to handle specific DAC channels

	Queue* wave_queue;						// Queue for receiving frequency instructions
	LUTQueue *lut_wave_queue;				// Queue for LUT changes
	EventFlag freq1_update_flag, freq2_update_flag;		// Flag for frequency updates
public:
	DACOutputDriver(DAC_HandleTypeDef*, TIM_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint32_t, Queue*, LUTQueue*, uint16_t);
	void update_DAC(void);					// Main function to update DAC channels
};


#endif /* INC_DACOUTPUTDRIVER_H_ */
