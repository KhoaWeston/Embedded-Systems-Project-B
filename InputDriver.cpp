/*
 * InputDriver.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: kcmartin
 */

#include "InputDriver.h"

InputDriver::InputDriver(Queue*){

}

InputDriver::getADD(){
	return ADD;
}

InputDriver::setup(){

}

InputDriver::Assign(gpio,gpio){

}

InputDriver::stateMachine(GPIO_TypeDef* gpio1, GPIO_TypeDef* gpio2){
	static uint8_t state = 0;
	bool printFlag;
	bool Astate = HAL_GPIO_ReadPin(GPIOA, GPIO_Pin_6);
	bool Bstate = HAL_GPIO_ReadPin(GPIOA, GPIO_Pin_7);
	switch (state) {
		default:
			state = 0;
			break;
		case 0:                         // Idle state, encoder not turning
			if (!Astate){             // Turn clockwise and A goes low first
				state = 1;
			} else if (!Bstate) {      // Turn anticlockwise and B goes low first
				state = 4;
			}
			break;
		// Clockwise rotation
		case 1:
			if (!Bstate) {             // Continue clockwise and B will go low after A
				state = 2;
			}
			break;
		case 2:
			if (Astate) {             // Turn further and A will go high first
				state = 3;
			}
			break;
		case 3:
			if (Astate && Bstate) {  // Both A and B now high as the encoder completes one step clockwise
				state = 0;
				printFlag = true; //increment after clockwise turn
			}
			break;
		// Anticlockwise rotation
		case 4:                         // As for clockwise but with A and B reversed
			if (!Astate) {
				state = 5;
			}
			break;
		case 5:
			if (Bstate) {
				state = 6;
			}
			break;
		case 6:
			if (Astate && Bstate) {
				state = 0;
				printFlag = true; //decrement after anticlockwise turn
			}
			break;
	}
	return printFlag;
}
