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

#define FREQ_KNOB_STEPS				(8)
#define SYS_CLOCK_FREQ				(80000000)   // 80 MHz system clock frequency
#define MAX_FREQ			        (50000)      // Frequency cap

class DACOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	DAC_HandleTypeDef* dac_handle; 		// Holds the specific DAC port
	TIM_HandleTypeDef* timer_handle; 	// Holds corresponding timer
	uint32_t dac_channel; 				// Holds the corresponding DAC channel constant
	uint32_t dac_alignment; 			// Holds the corresponding DAC alignment constant
	uint8_t curr_freq; 					// Holds the frequency in Hz
	uint32_t curr_wave_LUT[256];		//

	WaveQueue* queue;					// Holds the instructions to change frequency
	EventFlag* dac_update_flag;			// Flag for changing DAC output
	EventFlag* follow_mode_flag;		// Flag for switching to follow mode
	EventFlag* freq_update_flag;		// Flag to change timer frequency

	bool follow_mode_active;
	uint32_t follow_mode_freq;			// Holds the frequency to follow
public:
	DACOutputDriver(DAC_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint8_t, WaveQueue*, EventFlag*, EventFlag*, EventFlag*);
	void update_DAC(bool, uint32_t, uint32_t*, uint16_t);
	void reinitialize_timer(uint32_t, uint16_t);
	uint32_t get_freq(void);
};


#endif /* INC_DACOUTPUTDRIVER_H_ */
