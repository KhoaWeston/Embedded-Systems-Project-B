/*
 * OutputDriver.cpp
 *
 *  Created on: Sep 27, 2024
 *      Author: khoaw
 */

#include <DisplayOutputDriver.h>


DisplayOutputDriver::DisplayOutputDriver(I2C_HandleTypeDef* i2c, Queue* id_q, Queue* w_q){
	ASSERT(i2c != nullptr);
	ASSERT(id_q != nullptr);
	ASSERT(w_q != nullptr);

	i2c_handle = i2c;

	ID_queue = id_q;
	wave_queue = w_q;

	curr_channel = 1;
	selected_mode = 1;
	delay = 100;
	follow_mode = false;

	initialize_display();
}


// Initialize the OLED
void DisplayOutputDriver::initialize_display(void){
	// Retrieve initial values for both channels
	(void)wave_queue->dequeue(AMP1_1, amp1);
	(void)wave_queue->dequeue(AMP2_1, amp2);
	(void)wave_queue->dequeue(TYPE1_1, wave_type1);
	(void)wave_queue->dequeue(TYPE2_1, wave_type2);
	(void)wave_queue->dequeue(FREQ1_2, freq1);
	(void)wave_queue->dequeue(FREQ2_2, freq2);

	ASSERT(0 < amp1 && amp1 <= 3300);
	ASSERT(0 < amp2 && amp2 <= 3300);
	ASSERT(0 <= wave_type1 && wave_type1 <= 2);
	ASSERT(0 <= wave_type2 && wave_type2 <= 2);
	ASSERT(0 < freq1);
	ASSERT(0 < freq2);

	I2C_init(i2c_handle); 		// Initialize the OLED
	display_wave_type();
	display_channel();
	display_delay();
	display_freq();
	display_amp();

	for(uint8_t i=0; i < OLED_HEIGHT / PAGE_SIZE; i++) {
		update_screen(i); 		// Push updates to screen
	}
}


// Update the OLED display with current wave information
void DisplayOutputDriver::update_display(void){
	uint32_t fol_change;

	// Check for frequency/amplitude selection changes
	bool selection_updated = ID_queue->dequeue(F_OR_A, selected_mode);
	if(selection_updated){
		display_freq();
		display_amp();
	}

	// Check for channel selection changes
	bool channel_updated = ID_queue->dequeue(CHAN, curr_channel);
	if(channel_updated){
		bool follow_mode_updated = ID_queue->dequeue(FOLLOW_1, fol_change);
		if(follow_mode_updated){
			follow_mode = (fol_change == MODE_ON);
		}

		display_wave_type();
		display_channel();
		display_delay();
		display_freq();
		display_amp();
	}

	// Check for updates in amplitude, waveform type, frequency, or delay
	bool amp1_updated = wave_queue->dequeue(AMP1_1, amp1);
	bool amp2_updated = wave_queue->dequeue(AMP2_1, amp2);
	if(amp1_updated || amp2_updated){
		display_amp();
	}

	bool type1_updated = wave_queue->dequeue(TYPE1_1, wave_type1);
	bool type2_updated = wave_queue->dequeue(TYPE2_1, wave_type2);
	if(type1_updated || type2_updated){
		display_wave_type();
	}

	bool freq1_updated = wave_queue->dequeue(FREQ1_2, freq1);
	bool freq2_updated = wave_queue->dequeue(FREQ2_2, freq2);
	if(freq1_updated || freq2_updated){
		display_freq();
	}

	bool delay_updated = wave_queue->dequeue(DEL_1, delay);
	if(delay_updated){
		display_delay();
	}
}


// Display the selected channel
void DisplayOutputDriver::display_channel(void){
	ASSERT(curr_channel == 1 || curr_channel == 2);

	set_cursor(90, 0);
	write_string(("CH:"+std::to_string(curr_channel)).c_str());
	update_screen(0);
}


// Display the current waveform type
void DisplayOutputDriver::display_wave_type(void){
	uint8_t curr_wave_type = (curr_channel == 1) ? wave_type1-1 : wave_type2-1;
	ASSERT(0 <= curr_wave_type && curr_wave_type <= 2);

	static std::string wave_types[] = {"SINE  ", "SQUARE", "PULSE "}; // {"SINE  ", "SAW   ", "SQUARE", "TRI   ", "PULSE "}
	std::string wave_str = wave_types[curr_wave_type];

	set_cursor(0, 0);
	write_string(("WAVE:"+wave_str).c_str());
	update_screen(0);
}


// Display current frequency with one decimal place
void DisplayOutputDriver::display_freq(void){
	uint32_t curr_freq = (curr_channel == 1) ? freq1 : freq2;
	ASSERT(0 < curr_freq);

	std::string sel_arrow = (selected_mode == 1) ? ">" : "";
	std::string f_str = std::to_string(curr_freq);

	std::string f_str_dis;
	if(f_str.size()<4){
		f_str_dis = f_str + "Hz    ";
	}else{
		std::string thousandths = f_str.substr(0, f_str.size() - 3);
		std::string hundredths = f_str.substr(f_str.size() - 3, 1);
		f_str_dis = thousandths + "." + hundredths + "kHz ";
	}

	set_cursor(0, 16);
	write_string((sel_arrow + "FREQ: " + f_str_dis).c_str());
	update_screen(16/PAGE_SIZE);
}


// Display current amplitude with one decimal place
void DisplayOutputDriver::display_amp(void){
	uint16_t curr_amp = (curr_channel == 1) ? amp1 : amp2;
	ASSERT(0 < curr_amp && curr_amp <= 3300);

	std::string sel_arrow = (selected_mode == 2) ? ">" : "";
	std::string a_str = std::to_string(curr_amp);

	std::string a_str_dis;
	if(a_str.size()<4){
		a_str_dis = a_str + "mV ";
	}else{
		std::string thousandths = a_str.substr(0, a_str.size() - 3);
		std::string hundredths = a_str.substr(a_str.size() - 3, 1);
		a_str_dis = thousandths + "." + hundredths + "V  ";
	}

	set_cursor(0, 32);
	write_string((sel_arrow + "AMP: " + a_str_dis).c_str());
	update_screen(32/PAGE_SIZE);
}


// Display the delay percentage for CH2 when follower mode is active
void DisplayOutputDriver::display_delay(void){
	ASSERT(0 < delay && delay <= 100);

	std::string del_str;
	if(curr_channel == 2){
		if(!follow_mode){
			del_str = "OFF ";
		}else{
			del_str = (delay == 100) ? "0% " : std::to_string(delay)+"% ";
		}
	}else{
		del_str = "NA  ";
	}

	set_cursor(0, 48);
	write_string(("DELAY: "+del_str).c_str());
	update_screen(48/PAGE_SIZE);
}

