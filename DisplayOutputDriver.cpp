/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <DisplayOutputDriver.h>


// Initialize the Display class attributes
OutputDriverDisplay::OutputDriverDisplay(I2C_HandleTypeDef* i2c){
	i2c_num = i2c;
}


void OutputDriverDisplay::config(void){
	ssd1306_Init(i2c_num); // Initialize the OLED
	ssd1306_Fill(Black); // Set the background black
}


// Update the OLED display for all current wave information
void OutputDriverDisplay::update_display(uint8_t ch_num, uint8_t type, uint16_t freq, uint16_t amp, uint16_t h_off){
	ssd1306_SetCursor(0, 0);
	display_wave_type(type);
	ssd1306_SetCursor(90, 0);
	display_channel(ch_num);
	ssd1306_SetCursor(0, 20);
	display_freq(freq);
	ssd1306_SetCursor(0, 35);
	display_amp(amp);
	if(ch_num == 2){
		ssd1306_SetCursor(0, 50);
		display_hor_offset(h_off);
	}
	ssd1306_UpdateScreen(i2c_num);
}


void OutputDriverDisplay::display_channel(uint8_t ch_num){ // Display the channel most recently changed
	ssd1306_WriteString(("CH:"+std::to_string(ch_num)).c_str(), Font_7x10, Blue);
}


void OutputDriverDisplay::display_wave_type(uint8_t type){ // Display user-specified wave type
	std::string wave_type_str;
	switch(type){
		case 0 : wave_type_str = "Sine"; break;
		case 1 : wave_type_str = "Saw"; break;
		case 2 : wave_type_str = "Pulse"; break;
		case 3 : wave_type_str = "Tri"; break;
		default : wave_type_str = "Sine"; break;
	}

	ssd1306_WriteString(("Wave:"+wave_type_str).c_str(), Font_7x10, Blue);
}


void OutputDriverDisplay::display_freq(uint16_t freq){ // Display frequency to 1 decimal point resolution
	std::string f_str = std::to_string(freq);
	std::string thousandths = (f_str.size()==3) ? "0" : f_str.substr(0,f_str.size()-3);
	std::string hundredths = f_str.substr(f_str.size()-3,1);

	ssd1306_WriteString(("Freq.: "+thousandths+"."+hundredths+"kHz").c_str(), Font_7x10, Blue);
}


void OutputDriverDisplay::display_amp(uint16_t amp){ // Display the calculated amplitude
	std::string a_str = std::to_string((int)(amp*(3300/2048.)));
	std::string thousandths = (a_str.size()==3) ? "0" : a_str.substr(0,a_str.size()-3);
	std::string hundredths = a_str.substr(a_str.size()-3,1);

	ssd1306_WriteString(("Amplitude: "+thousandths+"."+hundredths+"V").c_str(), Font_7x10, Blue);
}


void OutputDriverDisplay::display_hor_offset(uint16_t h_off){ // Display the horizontal offset for CH2
	ssd1306_WriteString(("Hor. Offset: "+std::to_string(h_off)+"%").c_str(), Font_7x10, Blue);
}
