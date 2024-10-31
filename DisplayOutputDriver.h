/*
 * DisplayOutputDriver.h
 *
 *  Created on: Oct 3, 2024
 *      Author: khoaw
 */

#ifndef INC_DISPLAYOUTPUTDRIVER_H_
#define INC_DISPLAYOUTPUTDRIVER_H_

#include "main.h"
#include "ssd1306.h"
#include "Queue.h"
#include <string>


class DisplayOutputDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	OledI2CDriver OLED;
	I2C_HandleTypeDef* i2c_handle; 				// I2C handle for the OLED
	Queue* ID_queue, *wave_queue, *DAC_queue;	// Queues for updating values to display

	// Stores channel 1 and 2 values
	uint32_t selected_mode;						// Tells if frequency or amplitude is selected
	uint32_t curr_channel;						// Tells the current channel # that is selected
	uint32_t amp1, amp2, wave_type1, wave_type2, freq1, freq2, delay;
	bool follow_mode;							// Indicates if follow mode is active

	// OLED display functions
	void display_channel(void); 				// Display current channel
	void display_wave_type(void);				// Display current wave type
	void display_freq(void); 					// Display current frequency
	void display_amp(void);						// Display current amplitude
	void display_delay(void);					// Display current delay for CH2
	void initialize_display(void); 				// Initialize OLED with initial values
public:
	DisplayOutputDriver(I2C_HandleTypeDef*, Queue*, Queue*, Queue*);	// Constructor
	void update_display(void); 					// Update the OLED with latest wave parameters
};


#endif /* INC_DISPLAYOUTPUTDRIVER_H_ */
