/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <DisplayOutputDriver.h>


DisplayOutputDriver::DisplayOutputDriver(I2C_HandleTypeDef* i2c, Queue* q, Queue* q2, Queue* q3){
	i2c_handle = i2c;

	ID_queue = q;
	wave_queue = q2;
	DAC_queue = q3;

	curr_channel = 1;
	selected_mode = 1;
	delay = 100;
	follow_mode = false;

	initialize_display();
}


// Initialize the OLED
void DisplayOutputDriver::initialize_display(void){
	// Retrieve initial values for both channels
	wave_queue->dequeue(AMP1_1, amp1);
	wave_queue->dequeue(AMP2_1, amp2);
	wave_queue->dequeue(TYPE1_1, wave_type1);
	wave_queue->dequeue(TYPE2_1, wave_type2);
	DAC_queue->dequeue(FREQ1_1, freq1);
	DAC_queue->dequeue(FREQ2_1, freq2);

	ssd1306_Init(i2c_handle); // Initialize the OLED
	display_wave_type();
	display_channel();
	display_delay();
	display_freq();
	display_amp();
	ssd1306_UpdateScreen(i2c_handle); // Push updates to screen
}


// Update the OLED display with current wave information
void DisplayOutputDriver::update_display(){
	uint32_t fol_change;
	bool needs_screen_update = false;

	// Check for frequency/amplitude selection changes
	if(ID_queue->dequeue(F_OR_A, selected_mode)){
		display_freq();
		display_amp(); // update if amp, cursor, or channel

		needs_screen_update = true;
	}

	// Check for channel selection changes
	if(ID_queue->dequeue(CHAN, curr_channel)){
		if(ID_queue->dequeue(FOLLOW_2, fol_change)){
			follow_mode = (fol_change == MODE_ON);
		}

		display_wave_type();
		display_channel();
		display_delay();
		display_freq();
		display_amp();

		needs_screen_update = true;
	}

	// Check for updates in amplitude, waveform type, frequency, or delay
	if(wave_queue->dequeue(AMP1_1, amp1) || wave_queue->dequeue(AMP2_1, amp2)){
		display_amp();
		needs_screen_update = true;
	}
	if(wave_queue->dequeue(TYPE1_1, wave_type1) || wave_queue->dequeue(TYPE2_1, wave_type2)){
		display_wave_type();
		needs_screen_update = true;
	}
	if(DAC_queue->dequeue(FREQ1_1, freq1) || DAC_queue->dequeue(FREQ2_1, freq2)){
		display_freq();
		needs_screen_update = true;
	}
	if(wave_queue->dequeue(DEL_1, delay)){
		display_delay();
		needs_screen_update = true;
	}

	// Refresh screen if any changes were made
	if(needs_screen_update){
		ssd1306_UpdateScreen(i2c_handle);
	}
}


// Display the selected channel
void DisplayOutputDriver::display_channel(){
	ssd1306_SetCursor(90, 0);
	ssd1306_WriteString(("Ch:"+std::to_string(curr_channel)).c_str());
}


// Display the current waveform type
void DisplayOutputDriver::display_wave_type(){
	static std::string wave_types[] = {"ERROR ", "Sine  ", "Saw   ", "Square", "Tri   "};
	std::string wave_str = (curr_channel == 1) ? wave_types[wave_type1] : wave_types[wave_type2];

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(("Wave:"+wave_str).c_str());
}


// Display current frequency with one decimal place
void DisplayOutputDriver::display_freq(){
	std::string sel_arrow = (selected_mode == 1) ? ">" : "";

	std::string f_str = (curr_channel == 1) ? std::to_string(freq1) : std::to_string(freq2);
	std::string thousandths = (f_str.size()==3) ? "0" : f_str.substr(0,f_str.size()-3);
	std::string hundredths = f_str.substr(f_str.size()-3,1);

	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString((sel_arrow+"Freq.: "+thousandths+"."+hundredths+"kHz  ").c_str());
}


// Display current amplitude with one decimal place
void DisplayOutputDriver::display_amp(){
	std::string sel_arrow = (selected_mode == 2) ? ">" : "";

	std::string a_str = (curr_channel == 1) ? std::to_string(amp1) : std::to_string(amp2);
	std::string thousandths = (a_str.size()==3) ? "0" : a_str.substr(0,a_str.size()-3);
	std::string hundredths = a_str.substr(a_str.size()-3,1);

	ssd1306_SetCursor(0, 35);
	ssd1306_WriteString((sel_arrow+"Amp.: "+thousandths+"."+hundredths+"V  ").c_str());
}


// Display the delay percentage for CH2 when follower mode is active
void DisplayOutputDriver::display_delay(){
	ssd1306_SetCursor(0, 50);
	if(curr_channel == 2){
		std::string del_str = (follow_mode) ? std::to_string(delay)+"% " : "NA  ";
		ssd1306_WriteString(("Delay: "+del_str).c_str());
	}else{
		ssd1306_WriteString("                ");
	}
}

