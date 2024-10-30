/*
 * InputDriver.h
 *
 *  Created on: Oct 1, 2024
 *      Author: kcmartin
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_

#include "main.h"
#include "Button.h"
#include <stdint.h>
#include <stdio.h>

enum AmpFreq {Amp,Freq};
enum Channel {C1,C2};
enum Follow {OFF,ON};
enum Moves {NA,INC,DEC};

class InputDriver{
private:
	//WaveQueue *WaveQueue;		//fill ins until we have a queue finalized
	//DisplayQueue *DisplayQueue;
	//wave_params *wavemsg;
    //display params
	AmpFreq knob_toggle;
	Channel chan_toggle;
	Follow follow_toggle;
	GPIO_PinState btnLastState;
	volatile uint32_t tim_cnt;
	volatile uint32_t last_cnt;
public:
	InputDriver(void);
	void update(Button, Button, Button, Button);
	bool btnStateMachine(Button);
	Moves knobStateMachine(TIM_HandleTypeDef*);
};



#endif /* EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_ */
