/*
 * main_cpp.cpp
 *
 *  Created on: Sep 26, 2024
 *      Author: khoaw
 */

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "InputDriver.h"
#include "OutputDriver.h"

extern "C" void main_cpp(void){
	OutputDriver OD(GPIOA, GPIOB); // Creates an output driver for GPIOA and B

	while(1){
		;
	}
}