/*
 * DacOutputDriver.h
 *
 *  Created on: Oct 3, 2024
 *      Author: khoaw
 */

#ifndef INC_DACOUTPUTDRIVER_H_
#define INC_DACOUTPUTDRIVER_H_

#define F_KNOB_INTERVALS			(8)

#include "main.h"
#include "stm32l4xx_ll_tim.h"  // Offers the HAL_LL timer functions
#include "Queue.h"


class DACOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	DAC_HandleTypeDef* dac_port; 	// Holds the specific DAC port
	TIM_HandleTypeDef* tim_num; 	// Holds corresponding timer
	uint32_t dac_channel; 			// Holds the corresponding DAC channel constant
	uint32_t dac_alignment; 		// Holds the corresponding DAC alignment constant
	uint32_t freq; 					// Holds the desired frequency in Hz
	Queue* queue;					// Holds the instructions to change frequency

	uint32_t follow_freq;
public:
	DACOutputDriver(DAC_HandleTypeDef*, TIM_HandleTypeDef*, uint32_t, uint32_t, uint8_t, Queue*); // Initialize the DAC class members
	void update_DAC(/*bool, uint32_t, */uint32_t*, uint16_t); // Set the output value of the DAC
	uint32_t get_freq(void);
};


#endif /* INC_DACOUTPUTDRIVER_H_ */
