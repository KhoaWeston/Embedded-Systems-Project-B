/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <DisplayOutputDriver.h>

OutputDriverDisplay::OutputDriverDisplay(I2C_HandleTypeDef* i2c){
	i2c_num = i2c;
	wavetype = 0;
	frequency = 0;
	amplitude = 0;
}


void OutputDriverDisplay::config(){
	ssd1306_Init(i2c_num);
	HAL_Delay(1000);
	ssd1306_Fill(Black);
}


void OutputDriverDisplay::update_display(uint16_t type, uint16_t freq, uint16_t amp){
	wavetype = type;
	frequency = freq;
	amplitude = amp;

	ssd1306_SetCursor(0, 0);
	display_wave_type();
	ssd1306_SetCursor(0, 20);
	display_freq();
	ssd1306_SetCursor(0, 40);
	display_amp();
	ssd1306_UpdateScreen(i2c_num);
}


void OutputDriverDisplay::display_wave_type(){
	std::string wave_type_str;
	switch(wavetype){
		case 0 : wave_type_str = "Sine"; break;
		case 1 : wave_type_str = "Saw"; break;
		case 2 : wave_type_str = "Pulse"; break;
		case 3 : wave_type_str = "Tri"; break;
		default : wave_type_str = "Sine"; break;
	}

	ssd1306_WriteString(("Wave: "+wave_type_str).c_str(), Font_11x18, Blue);
}


void OutputDriverDisplay::display_freq(){
	std::string f_str = std::to_string(frequency);

	ssd1306_WriteString(("f: "+f_str.substr(0,1)+"."+f_str.substr(1,1)+"kHz").c_str(), Font_11x18, Blue);
}


void OutputDriverDisplay::display_amp(){
	ssd1306_WriteString(("Amp: "+std::to_string((int)(amplitude*(336/2048.)))+"mV").c_str(), Font_11x18, Blue);
}


