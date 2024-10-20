/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <DisplayOutputDriver.h>


DisplayOutputDriver::DisplayOutputDriver(I2C_HandleTypeDef* i2c, DisplayQueue* q){
	i2c_handle = i2c;
	queue = q;
	curr_channel=1;
	selected_mode = 1;
}


// Initialize the OLED
void DisplayOutputDriver::initialize_display(void){
	ssd1306_Init(i2c_handle); // Initialize the OLED
	ssd1306_Fill(Black); // Set the background black
	ssd1306_UpdateScreen(i2c_handle);
}


// Update the OLED display for all current wave information
void DisplayOutputDriver::update_display(Wave* wave1, Wave* wave2, uint16_t freq1, uint16_t freq2){
	bool FA_flag = queue->dequeue(&selected_mode, F_OR_A); // Tells if selection has changed between frequency and amplitude
	bool chan_flag = queue->dequeue(&curr_channel, CHAN); // Tells if selection has changed between channels

	if(chan_flag || FA_flag){
		ssd1306_SetCursor(0, 0);
		display_wave_type((curr_channel==1) ? wave1->get_wave_type() : wave2->get_wave_type());
		ssd1306_SetCursor(90, 0);
		display_channel();
		ssd1306_SetCursor(0, 20);
		display_freq((curr_channel==1) ? freq1 : freq2);
		ssd1306_SetCursor(0, 35);
		display_amp((curr_channel==1) ? wave1->get_amplitude() : wave2->get_amplitude());
		ssd1306_SetCursor(0, 50);
		display_delay(wave2->is_follow_mode_active(), wave2->get_delay());
		ssd1306_UpdateScreen(i2c_handle);
	}
}


// Display the channel most recently changed
void DisplayOutputDriver::display_channel(){
	ssd1306_WriteString(("Ch:"+std::to_string(curr_channel)).c_str(), Font_7x10, Blue);
}


// Display the current wave type
void DisplayOutputDriver::display_wave_type(uint8_t type){
	static std::string wave_types[] = { "Sine  ", "Saw   ", "Square", "Tri   " };

	ssd1306_WriteString(("Wave:"+wave_types[type]).c_str(), Font_7x10, Blue);
}


// Display frequency to 1 decimal point resolution
void DisplayOutputDriver::display_freq(uint32_t freq){
	std::string sel_arrow = (selected_mode == 1) ? ">" : "";
	std::string f_str = std::to_string(freq);
	std::string thousandths = (f_str.size()==3) ? "0" : f_str.substr(0,f_str.size()-3);
	std::string hundredths = f_str.substr(f_str.size()-3,1);

	ssd1306_WriteString((sel_arrow+"Freq.: "+thousandths+"."+hundredths+"kHz  ").c_str(), Font_7x10, Blue);
}


// Display the calculated amplitude
void DisplayOutputDriver::display_amp(uint16_t amp){
	std::string sel_arrow = (selected_mode == 2) ? ">" : "";
	uint16_t calculated_amp = static_cast<uint16_t>(amp*(3300/2048.));
	std::string a_str = std::to_string(calculated_amp);
	std::string thousandths = (a_str.size()==3) ? "0" : a_str.substr(0,a_str.size()-3);
	std::string hundredths = a_str.substr(a_str.size()-3,1);

	ssd1306_WriteString((sel_arrow+"Amp.: "+thousandths+"."+hundredths+"V  ").c_str(), Font_7x10, Blue);
}


// Display the delay for CH2 when in follower mode
void DisplayOutputDriver::display_delay(bool follow_mode, uint16_t delay){
	if(curr_channel == 1){
		ssd1306_WriteString("                ", Font_7x10, Blue);
	}else{
		std::string del_str = (follow_mode) ? std::to_string(delay)+"% " : "NA ";
		ssd1306_WriteString(("Delay: "+del_str).c_str(), Font_7x10, Blue);
	}
}

