/*
 * DisplayOutputDriver.h
 *
 *  Created on: Oct 3, 2024
 *      Author: khoaw
 */

#ifndef INC_DISPLAYOUTPUTDRIVER_H_
#define INC_DISPLAYOUTPUTDRIVER_H_


#include "main.h"
#include "fonts.h"
#include "ssd1306.h"
#include <string>
#include "math.h"
#include "Wave.h"
#include "Queue.h"

class OutputDriverDisplay{ // @suppress("Miss copy constructor or assignment operator")
private:
	I2C_HandleTypeDef* i2c_num; 	// Holds the I2C object
	Dis_queue* queue;
	uint8_t FA_sel;
	uint8_t ch_num;
public:
	OutputDriverDisplay(I2C_HandleTypeDef*, Dis_queue*); 	// Initialize the Display class attributes
	void config(void); 							// Initialize the OLED
	void update_display(Wave*, Wave*, uint16_t, uint16_t); 	// Update the OLED display for all current wave information
	void display_channel(); 				// Display the channel most recently changed
	void display_wave_type(uint8_t);				// Display user-specified wave type
	void display_freq(uint16_t); 					// Display frequency to 1 decimal point resolution
	void display_amp(uint16_t);						// Display the calculated amplitude
	void display_hor_offset(bool, uint16_t);				// Display the horizontal offset for CH2
};


#endif /* INC_DISPLAYOUTPUTDRIVER_H_ */
