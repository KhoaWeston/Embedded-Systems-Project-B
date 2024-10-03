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


class OutputDriverDisplay{
private:
	I2C_HandleTypeDef* i2c_num;
	uint16_t wavetype;
	uint16_t frequency;
	uint16_t amplitude;
public:
	OutputDriverDisplay(I2C_HandleTypeDef*);
	void config();
	void update_display(uint16_t, uint16_t, uint16_t);
	void display_wave_type();
	void display_freq();
	void display_amp();
};


#endif /* INC_DISPLAYOUTPUTDRIVER_H_ */
