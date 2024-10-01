/*
 * InputDriver.h
 *
 *  Created on: Oct 1, 2024
 *      Author: kcmartin
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_

#include "main.h"

class InputDriver{
private:
	Queue* queue;
	move next;
	GPIO_PinState incLastState;
	GPIO_PinState decLastState;
public:
	InputDriver(Queue*);
	int32_t getADD();
	void setup();
	void Assign(gpio,gpio);
	bool stateMachine(gpio);
};



#endif /* EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_ */
