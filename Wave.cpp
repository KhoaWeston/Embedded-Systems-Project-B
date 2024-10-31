/*
 * Wave.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: khoaw
 */

#include "Wave.h"


Wave::Wave(Queue* q, LUTQueue* q2, Queue* q3){ // @suppress("Class members should be properly initialized")
	ID_queue = q;
	DAC_queue = q2;
	dis_queue = q3;

	is_in_follow_mode = false;
	delay = DELAY_KNOB_STEPS;

	// Set external dependencies for each wave child
	wave_ch1.set_pointers(ID_queue, &wave1_update_flag);
	wave_ch2.set_pointers(ID_queue, &wave2_update_flag);

	// Initialize DAC and display with initial waveforms and parameters
	DAC_queue->enqueue(LUT1, (uint32_t*)wave_ch1.get_active_wave_LUT());
	DAC_queue->enqueue(LUT2, (uint32_t*)wave_ch2.get_active_wave_LUT());

	dis_queue->enqueue(AMP1_1, wave_ch1.get_amplitude());
	dis_queue->enqueue(TYPE1_1, wave_ch1.get_wave_type());
	dis_queue->enqueue(AMP2_1, wave_ch2.get_amplitude());
	dis_queue->enqueue(TYPE2_1, wave_ch2.get_wave_type());
}


void Wave::update_waves(void){
	// Update wave parameters for each child
	wave_ch1.update_wave_params(TYPE1_0, AMP1_0);
	wave_ch2.update_wave_params(TYPE2_0, AMP2_0);

	uint32_t follow_mode_change, delay_change;

	// Toggle follow mode
	if(ID_queue->dequeue(FOLLOW_0, follow_mode_change)){
		//assert(!(follow_mode_change == true || follow_mode_change == false));
		is_in_follow_mode = (follow_mode_change == MODE_ON);
		wave2_update_flag.set_flag();

		if(is_in_follow_mode){
			shift_follow_wave();
		}else{
			wave_ch2.write_output_wave();
		}
	}

	// Synchronize Channel 2 with Channel 1 if in follow mode
	if(wave1_update_flag.is_flag_set() && is_in_follow_mode){
		wave2_update_flag.set_flag();
		shift_follow_wave();
	}

	// Adjust delay of wave 2 while in follow mode
	if(ID_queue->dequeue(DEL_0, delay_change)){
		//assert(!(delay_change == INC || delay_change == DEC));
		if(delay_change == INC){
			delay = (delay == DELAY_KNOB_STEPS) ? 1 : delay+1;
		}else if(delay_change == DEC){
			delay = (delay == 1) ? DELAY_KNOB_STEPS : delay-1;
		}
		wave2_update_flag.set_flag();
		shift_follow_wave();

		uint16_t delay_in_percent = static_cast<uint16_t>((delay / (float)AMP_KNOB_STEPS) * 100);
		dis_queue->enqueue(DEL_1, delay_in_percent);
	}

	// Update display if wave settings have changed
	if(wave1_update_flag.is_flag_set()){
		wave1_update_flag.reset_flag();
		dis_queue->enqueue(AMP1_1, wave_ch1.get_amplitude());
		dis_queue->enqueue(TYPE1_1, wave_ch1.get_wave_type());
	}
	if(wave2_update_flag.is_flag_set()){
		wave2_update_flag.reset_flag();
		if(is_in_follow_mode){
			dis_queue->enqueue(AMP2_1, wave_ch1.get_amplitude());
			dis_queue->enqueue(TYPE2_1, wave_ch1.get_wave_type());
		}else{
			dis_queue->enqueue(AMP2_1, wave_ch2.get_amplitude());
			dis_queue->enqueue(TYPE2_1, wave_ch2.get_wave_type());
		}
	}
}


void Wave::shift_follow_wave(void){
	uint32_t* base_LUT = wave_ch1.get_active_wave_LUT();
	uint32_t* shifted_LUT = wave_ch2.get_active_wave_LUT();
	uint16_t shift_idx = (delay / (float)DELAY_KNOB_STEPS) * LUT_SIZE;

	for (uint16_t i = 0; i < LUT_SIZE; i++){
		shifted_LUT[i] = base_LUT[(i + shift_idx) % LUT_SIZE];
	}
}


Wave::IndWave::IndWave(){ // @suppress("Class members should be properly initialized")
	curr_wave = SINE;
	curr_amp = 1;
}


void Wave::IndWave::write_output_wave(void){
	float scale_factor = curr_amp / (float)AMP_KNOB_STEPS;
	const uint32_t* wave_table = base_wave_tables[curr_wave - 1];

	for (uint16_t i = 0; i < LUT_SIZE; i++) {
		output_wave_LUT[i] = wave_table[i] * scale_factor + VERT_OFFSET;
	}
}


void Wave::IndWave::set_pointers(Queue *q, EventFlag* f){
	queue = q;
	wave_update_flag = f;

	write_output_wave();
}


void Wave::IndWave::update_wave_params(uint8_t w_num, uint8_t a_num){ // Update wave parameters from queue
	uint32_t wave_change, amp_change;

	// Cycle through wave types
	bool waveform_updated = queue->dequeue(w_num, wave_change);
	if(waveform_updated && wave_change == INC){
		//assert(!(wave_change == INC));
		curr_wave = static_cast<WaveType>((curr_wave % 4) + 1);
		write_output_wave();
		wave_update_flag->set_flag();
	}

	// Change amplitude of all wave tables
	if(queue->dequeue(a_num, amp_change)){
		//assert(!(amp_change == INC || amp_change == DEC));

		if(amp_change == INC){
			curr_amp = (curr_amp == AMP_KNOB_STEPS) ? 1 : curr_amp+1;
		}else if(amp_change == DEC){
			curr_amp = (curr_amp == 1) ? AMP_KNOB_STEPS : curr_amp-1;
		}
		write_output_wave();
		wave_update_flag->set_flag();
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


// Pre-computed lookup tables
const uint32_t sine_wave_table[LUT_SIZE] = {
1998, 2096, 2193, 2291, 2387, 2483, 2577, 2671, 2762, 2852, 2939, 3025, 3108, 3188, 3265, 3339, 3410, 3478, 3542, 3602,
3659, 3711, 3760, 3804, 3843, 3879, 3909, 3936, 3957, 3974, 3986, 3993, 3996, 3993, 3986, 3974, 3957, 3936, 3909, 3879,
3843, 3804, 3760, 3711, 3659, 3602, 3542, 3478, 3410, 3339, 3265, 3188, 3108, 3025, 2939, 2852, 2762, 2671, 2577, 2483,
2387, 2291, 2193, 2096, 1998, 1899, 1802, 1704, 1608, 1512, 1418, 1324, 1233, 1143, 1056, 970, 887, 807, 730, 656, 585,
517, 453, 393, 336, 284, 235, 191, 152, 116, 86, 59, 38, 21, 9, 2, 0, 2, 9, 21, 38, 59, 86, 116, 152, 191, 235, 284, 336,
393, 453, 517, 585, 656, 730, 807, 887, 970, 1056, 1143, 1233, 1324, 1418, 1512, 1608, 1704, 1802, 1899
};

const uint32_t saw_wave_table[LUT_SIZE] = {
0, 31, 62, 94, 125, 157, 188, 220, 251, 283, 314, 346, 377, 409, 440, 471, 503, 534, 566, 597, 629, 660, 692,
723, 755, 786, 818, 849, 881, 912, 943, 975, 1006, 1038, 1069, 1101, 1132, 1164, 1195, 1227, 1258, 1290, 1321,
1352, 1384, 1415, 1447, 1478, 1510, 1541, 1573, 1604, 1636, 1667, 1699, 1730, 1762, 1793, 1824, 1856, 1887,
1919, 1950, 1982, 2013, 2045, 2076, 2108, 2139, 2171, 2202, 2233, 2265, 2296, 2328, 2359, 2391, 2422, 2454,
2485, 2517, 2548, 2580, 2611, 2643, 2674, 2705, 2737, 2768, 2800, 2831, 2863, 2894, 2926, 2957, 2989, 3020,
3052, 3083, 3114, 3146, 3177, 3209, 3240, 3272, 3303, 3335, 3366, 3398, 3429, 3461, 3492, 3524, 3555, 3586,
3618, 3649, 3681, 3712, 3744, 3775, 3807, 3838, 3870, 3901, 3933, 3964, 3996
};

const uint32_t square_wave_table[LUT_SIZE] = {
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996, 3996,
3996, 3996, 3996, 3996, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint32_t triangle_wave_table[LUT_SIZE] = {
3996, 3933, 3871, 3808, 3746, 3683, 3621, 3558, 3496, 3434, 3371, 3309, 3246, 3184, 3121, 3059, 2997, 2934, 2872, 2809, 2747,
2684, 2622, 2559, 2497, 2435, 2372, 2310, 2247, 2185, 2122, 2060, 1998, 1935, 1873, 1810, 1748, 1685, 1623, 1560, 1498, 1436,
1373, 1311, 1248, 1186, 1123, 1061, 999, 936, 874, 811, 749, 686, 624, 561, 499, 437, 374, 312, 249, 187, 124, 62, 0, 62, 124,
187, 249, 312, 374, 437, 499, 561, 624, 686, 749, 811, 874, 936, 999, 1061, 1123, 1186, 1248, 1311, 1373, 1436, 1498, 1560,
1623, 1685, 1748, 1810, 1873, 1935, 1998, 2060, 2122, 2185, 2247, 2310, 2372, 2435, 2497, 2559, 2622, 2684, 2747, 2809, 2872,
2934, 2997, 3059, 3121, 3184, 3246, 3309, 3371, 3434, 3496, 3558, 3621, 3683, 3746, 3808, 3871, 3933
};

// Group tables for ease of access in code
const uint32_t* base_wave_tables[4] = {sine_wave_table, saw_wave_table, square_wave_table, triangle_wave_table};
