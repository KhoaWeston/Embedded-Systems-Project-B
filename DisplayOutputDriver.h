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
#include <string>


/**
 * @brief Manages OLED display output for showing current wave parameters.
 *
 * Interfaces with an OLED display to show parameters such as
 * channel, wave type, frequency, amplitude, and delay.
 */
class DisplayOutputDriver : private OledI2cDriver { // @suppress("Miss copy constructor or assignment operator")
private:
	I2C_HandleTypeDef* i2c_handle;			// I2C handle for the OLED
	Queue* ID_queue, *wave_queue;			// Queues for updating values to display

	// Stores channel 1 and 2 values
	uint32_t selected_mode;					// Tells if frequency or amplitude is selected
	uint32_t curr_channel;					// Tells the current channel # that is selected
	uint32_t amp1;
	uint32_t amp2;
	uint32_t wave_type1;
	uint32_t wave_type2;
	uint32_t freq1;
	uint32_t freq2;
	uint32_t delay;
	bool follow_mode;						// Indicates if follow mode is active

	/**
	 * @brief Displays the currently selected channel on the OLED.
	 */
	void display_channel(void);

	/**
	 * @brief Displays the currently selected wave type on the OLED.
	 */
	void display_wave_type(void);

	/**
	 * @brief Displays the current frequency value for the selected channel in volts.
	 */
	void display_freq(void);

	/**
	 * @brief Displays the current amplitude value for the selected channel in hertz.
	 */
	void display_amp(void);

	/**
	 * @brief Displays the current delay value for channel 2 in percent.
	 */
	void display_delay(void);

	/**
	 * @brief Initializes the OLED display with initial default values.
	 *
	 * Sets up the display to show basic information before any updates
	 * from the queues are received.
	 */
	void initialize_display(void);
public:
	/**
	 * @brief Constructs the DisplayOutputDriver object.
	 * @param x i2c Pointer to the I2C handle for the OLED display.
	 * @param x id_q Pointer to the queue for channel ID updates.
	 * @param x w_q Pointer to the queue for waveform parameter updates.
	 */
	DisplayOutputDriver(I2C_HandleTypeDef* i2c, Queue* id_q, Queue* w_q);

	/**
	 * @brief Updates the OLED display with the latest waveform parameters.
	 *
	 * Reads from the ID_queue and wave_queue to retrieve the most recent
	 * values for frequency, amplitude, wave type, delay, and channel selection.
	 */
	void update_display(void);
};


#endif /* INC_DISPLAYOUTPUTDRIVER_H_ */
