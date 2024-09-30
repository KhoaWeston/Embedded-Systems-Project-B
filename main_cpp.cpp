/*
 * main_cpp.cpp
 *
 *  Created on: Sep 26, 2024
 *      Author: khoaw
 */

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "OutputDriver.h"
#include "stm32l4xx_ll_dac.h"


extern "C" void main_cpp(void){
	uint8_t sineIndex = 0;
	uint16_t sineWave[];

	DAC_HandleTypeDef hdac1;
	OutputDriver OD1(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R);
	OD1.config();

	while(1){
		OD1.update(sineWave[sineIndex++]);

		HAL_Delay(1);
	}
}

