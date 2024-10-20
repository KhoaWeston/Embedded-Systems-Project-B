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
#include "Wave.h"
#include "Queue.h"
#include "math.h"
#include <string>


class DisplayOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	I2C_HandleTypeDef* i2c_handle; 	// Holds the I2C object
	DisplayQueue* queue;				// Holds the instructions for what to display
	uint8_t selected_mode;					// Holds whether frequency or amplitude to selected
	uint8_t curr_channel;					// Holds the current channel # that is selected
public:
	DisplayOutputDriver(I2C_HandleTypeDef*, DisplayQueue*);
	void initialize_display(void); 										// Initialize the OLED
	void update_display(Wave*, Wave*, uint16_t, uint16_t); 	// Update the OLED display for all current wave information
	void display_channel(void); 								// Display the channel most recently changed
	void display_wave_type(uint8_t);						// Display the current wave type
	void display_freq(uint32_t); 							// Display frequency to 1 decimal point resolution
	void display_amp(uint16_t);								// Display the calculated amplitude
	void display_delay(bool, uint16_t);						// Display the delay for CH2 when in follower mode
};


#endif /* INC_DISPLAYOUTPUTDRIVER_H_ */
