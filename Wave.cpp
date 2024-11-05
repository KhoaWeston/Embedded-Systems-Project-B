/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(Queue* q1, Queue* q2, LUTQueue* q3, Queue* q4){ // @suppress("Class members should be properly initialized")
	ID_queue = q1;
	DAC_queue = q2;
	LUT_DAC_queue = q3;
	dis_queue = q4;

	is_in_follow_mode = false;
	delay = DELAY_KNOB_STEPS;

	initialize_parent();
}


void Wave::initialize_parent(void){
	// Set external dependencies for each wave child
	wave_ch1.initialize(ID_queue, &wave1_update_flag, &freq1_update_flag);
	wave_ch2.initialize(ID_queue, &wave2_update_flag, &freq2_update_flag);

	bool status = true;

	// Initialize DAC with initial waveforms and parameters
	status &= LUT_DAC_queue->enqueue(LUT1, (uint32_t*)wave_ch1.get_active_wave_LUT());
	status &= LUT_DAC_queue->enqueue(LUT2, (uint32_t*)wave_ch2.get_active_wave_LUT());
	status &= DAC_queue->enqueue(FREQ1_1, wave_ch1.get_freq_Hz());
	status &= DAC_queue->enqueue(FREQ2_1, wave_ch2.get_freq_Hz());

	// Initialize display with starting parameters
	status &= dis_queue->enqueue(AMP1_1, wave_ch1.get_amplitude());
	status &= dis_queue->enqueue(AMP2_1, wave_ch2.get_amplitude());
	status &= dis_queue->enqueue(TYPE1_1, wave_ch1.get_wave_type());
	status &= dis_queue->enqueue(TYPE2_1, wave_ch2.get_wave_type());
	status &= dis_queue->enqueue(FREQ1_2, wave_ch1.get_freq_Hz());
	status &= dis_queue->enqueue(FREQ2_2, wave_ch2.get_freq_Hz());

	if(!status){
		NVIC_SystemReset();
	}
}


void Wave::update_waves(void){
	// Update wave parameters for each child
	wave_ch1.update_wave_params(TYPE1_0, AMP1_0, FREQ1_0);
	wave_ch2.update_wave_params(TYPE2_0, AMP2_0, FREQ2_0);

	uint32_t follow_mode_change, delay_change;

	// Toggle follow mode
	bool follow_mode_updated = ID_queue->dequeue(FOLLOW_0, follow_mode_change);
	if(follow_mode_updated){
		ASSERT(follow_mode_change == MODE_ON || follow_mode_change == MODE_OFF);

		is_in_follow_mode = (follow_mode_change == MODE_ON);
		wave2_update_flag.set_flag();
		freq2_update_flag.set_flag();

		// Handle wave output based on follow mode
		if(is_in_follow_mode){
			shift_follow_wave();
		}else{
			wave_ch2.write_output_wave();
		}
	}

	// Adjust delay of wave 2 while in follow mode
	bool delay_updated = ID_queue->dequeue(DEL_0, delay_change);
	if(delay_updated){
		ASSERT(delay_change == INC || delay_change == DEC);

		if(delay_change == INC){
			delay = (delay == DELAY_KNOB_STEPS) ? 1 : delay+1;
		}else if(delay_change == DEC){
			delay = (delay == 1) ? DELAY_KNOB_STEPS : delay-1;
		}
		shift_follow_wave();

		uint16_t delay_in_percent = static_cast<uint16_t>((delay / (float)DELAY_KNOB_STEPS) * 100);
		dis_queue->enqueue(DEL_1, delay_in_percent);
	}

	// Update display if wave settings have changed
	if(wave1_update_flag.is_flag_set()){
		wave1_update_flag.reset_flag();

		// Synchronize Channel 2 with Channel 1 if in follow mode
		if(is_in_follow_mode){
			wave2_update_flag.set_flag();
			shift_follow_wave();
		}

		dis_queue->enqueue(AMP1_1, wave_ch1.get_amplitude());
		dis_queue->enqueue(TYPE1_1, wave_ch1.get_wave_type());
	}
	if(wave2_update_flag.is_flag_set()){
		wave2_update_flag.reset_flag();
//		bool amp_sent, type_sent;
		if(is_in_follow_mode){
			dis_queue->enqueue(AMP2_1, wave_ch1.get_amplitude());
			dis_queue->enqueue(TYPE2_1, wave_ch1.get_wave_type());
		}else{
			dis_queue->enqueue(AMP2_1, wave_ch2.get_amplitude());
			dis_queue->enqueue(TYPE2_1, wave_ch2.get_wave_type());
		}
	}

	// If channels are changed, sync DACs and update displays with new frequencies
	if(freq1_update_flag.is_flag_set()){
		freq1_update_flag.reset_flag();

		// Reinitialize channel 2 timer if frequency channel 1 is adjusted in follower mode
		if(is_in_follow_mode){
			freq2_update_flag.set_flag();
		}

		uint32_t wave1_freq = wave_ch1.get_freq_Hz();
		ASSERT(0 < wave1_freq && wave1_freq <= MAX_FREQ);

		DAC_queue->enqueue(FREQ1_1, wave1_freq);
		dis_queue->enqueue(FREQ1_2, wave1_freq);
	}
	if(freq2_update_flag.is_flag_set()){
		freq2_update_flag.reset_flag();

		if(is_in_follow_mode){
			uint32_t wave1_freq = wave_ch1.get_freq_Hz();
			ASSERT(0 < wave1_freq && wave1_freq <= MAX_FREQ);

			DAC_queue->enqueue(FREQ2_1, wave1_freq);
			dis_queue->enqueue(FREQ2_2, wave1_freq);
		}else{
			uint32_t wave2_freq = wave_ch2.get_freq_Hz();
			ASSERT(0 < wave2_freq && wave2_freq <= MAX_FREQ);

			DAC_queue->enqueue(FREQ2_1, wave2_freq);
			dis_queue->enqueue(FREQ2_2, wave2_freq);
		}
	}
}


void Wave::shift_follow_wave(void){
	uint32_t* base_LUT = wave_ch1.get_active_wave_LUT();
	uint32_t* shifted_LUT = wave_ch2.get_active_wave_LUT();
	uint16_t shift_idx = (delay / (float)DELAY_KNOB_STEPS) * LUT_SIZE;

	if (shift_idx == 0) { // If shift_idx is zero, copy directly
		for (uint16_t i = 0; i < LUT_SIZE; i++) {
			shifted_LUT[i] = base_LUT[i];
		}
	}else{
		// Handle the wraparound manually
		uint16_t new_index;
		for (uint16_t i = 0; i < LUT_SIZE; i++) {
			new_index = i + shift_idx;
			if (new_index >= LUT_SIZE) {
				new_index -= LUT_SIZE; // Manually wrap around
			}
			shifted_LUT[i] = base_LUT[new_index];
		}
	}
}


Wave::IndWave::IndWave(){ // @suppress("Class members should be properly initialized")
	curr_wave = SINE;
	curr_amp = 1;
	curr_freq = 1;

	write_output_wave();
}


void Wave::IndWave::write_output_wave(void){
	float scale_factor = curr_amp / (float)AMP_KNOB_STEPS;
	const uint32_t* wave_table = base_wave_tables[curr_wave - 1];

	for (uint16_t i = 0; i < LUT_SIZE; i++) {
		output_wave_LUT[i] = wave_table[i] * scale_factor + VERT_OFFSET;
	}
}


void Wave::IndWave::initialize(Queue *q, EventFlag* w, EventFlag* f){
	ID_queue = q;
	wave_update_flag = w;
	freq_update_flag = f;
}


void Wave::IndWave::update_wave_params(uint8_t w_num, uint8_t a_num, uint8_t f_num){ // Update wave parameters from queue
	uint32_t wave_change, amp_change, freq_change;

	// Cycle through wave types
	bool type_updated = ID_queue->dequeue(w_num, wave_change);
	if(type_updated){
		ASSERT(wave_change == INC);

		curr_wave = static_cast<WaveType>((curr_wave % NUM_WAVE_TYPES) + 1);
		write_output_wave();
		wave_update_flag->set_flag();
	}

	// Change amplitude of all wave tables
	bool amp_updated = ID_queue->dequeue(a_num, amp_change);
	if(amp_updated){
		ASSERT(amp_change == INC || amp_change == DEC);

		if(amp_change == INC){
			curr_amp = (curr_amp == AMP_KNOB_STEPS) ? 1 : curr_amp+1;
		}else if(amp_change == DEC){
			curr_amp = (curr_amp == 1) ? AMP_KNOB_STEPS : curr_amp-1;
		}
		write_output_wave();
		wave_update_flag->set_flag();
	}

	// Adjust the timer frequency
	bool freq_udpated = ID_queue->dequeue(f_num, freq_change);
	if(freq_udpated){
		ASSERT(freq_change == INC || freq_change == DEC);

		if(freq_change == INC){
			curr_freq = (curr_freq == FREQ_KNOB_STEPS) ? 1 : curr_freq+1;
		}else if(freq_change == DEC){
			curr_freq = (curr_freq == 1) ? FREQ_KNOB_STEPS : curr_freq-1;
		}
		freq_update_flag->set_flag();
	}
}


uint32_t* Wave::IndWave::get_active_wave_LUT(void){ // Return the current wave lookup table
	return output_wave_LUT;
}


uint16_t Wave::IndWave::get_amplitude(void) {
	const uint16_t max_dac_out = 3300; // in mV
    return static_cast<uint16_t>((curr_amp / (float)AMP_KNOB_STEPS) * max_dac_out);
}


WaveType Wave::IndWave::get_wave_type(void) {
    return curr_wave;
}


uint32_t Wave::IndWave::get_freq_Hz(void){
    return (curr_freq == 1) ? 1 : static_cast<uint32_t>(((float)curr_freq / FREQ_KNOB_STEPS) * MAX_FREQ);
}


// Pre-computed lookup tables
const uint32_t sine_wave_table[LUT_SIZE] = {
1998, 2047, 2096, 2144, 2193, 2242, 2291, 2339, 2387, 2435, 2483, 2530, 2577, 2624, 2671, 2717, 2762, 2807, 2852, 2896,
2939, 2982, 3025, 3066, 3108, 3148, 3188, 3227, 3265, 3303, 3339, 3375, 3410, 3445, 3478, 3510, 3542, 3573, 3602, 3631,
3659, 3686, 3711, 3736, 3760, 3782, 3804, 3824, 3843, 3862, 3879, 3895, 3909, 3923, 3936, 3947, 3957, 3966, 3974, 3980,
3986, 3990, 3993, 3995, 3996, 3995, 3993, 3990, 3986, 3980, 3974, 3966, 3957, 3947, 3936, 3923, 3909, 3895, 3879, 3862,
3843, 3824, 3804, 3782, 3760, 3736, 3711, 3686, 3659, 3631, 3602, 3573, 3542, 3510, 3478, 3445, 3410, 3375, 3339, 3303,
3265, 3227, 3188, 3148, 3108, 3066, 3025, 2982, 2939, 2896, 2852, 2807, 2762, 2717, 2671, 2624, 2577, 2530, 2483, 2435,
2387, 2339, 2291, 2242, 2193, 2144, 2096, 2047, 1998, 1948, 1899, 1851, 1802, 1753, 1704, 1656, 1608, 1560, 1512, 1465,
1418, 1371, 1324, 1278, 1233, 1188, 1143, 1099, 1056, 1013, 970, 929, 887, 847, 807, 768, 730, 692, 656, 620, 585, 550,
517, 485, 453, 422, 393, 364, 336, 309, 284, 259, 235, 213, 191, 171, 152, 133, 116, 100, 86, 72, 59, 48, 38, 29, 21, 15,
9, 5, 2, 0, 0, 0, 2, 5, 9, 15, 21, 29, 38, 48, 59, 72, 86, 100, 116, 133, 152, 171, 191, 213, 235, 259, 284, 309, 336, 364,
393, 422, 453, 485, 517, 550, 585, 620, 656, 692, 730, 768, 807, 847, 887, 929, 970, 1013, 1056, 1099, 1143, 1188, 1233,
1278, 1324, 1371, 1418, 1465, 1512, 1560, 1608, 1656, 1704, 1753, 1802, 1851, 1899, 1948
};


const uint32_t square_wave_table[LUT_SIZE] = {
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


const uint32_t pulse_wave_table[LUT_SIZE] = {
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


// Group tables for ease of access in code
const uint32_t* base_wave_tables[4] = {sine_wave_table, square_wave_table, pulse_wave_table};
