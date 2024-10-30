/*
 * InputDriver.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: kcmartin
 */

#include "InputDriver.h"

#define BTN_PRESSED GPIO_PIN_RESET //GPIO_PIN_RESET = 0
#define BTN_DEPRESSED GPIO_PIN_SET //GPIO_PIN_SET = 1



InputDriver::InputDriver(void){
	btnLastState=BTN_DEPRESSED;
	knob_toggle = AMP;
	chan_toggle = C1;
	follow_toggle = OFF;
	tim_cnt = 0;
	last_cnt = 0;
}

//////////Update the Driver, Send Data to Queues//////////
InputDriver::update(Button AmpFreq, Button Shape, Button Channel, Button Tshift, TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2){
	if (btnStateMachine(AmpFreq) == true){
		//toggle the amp/freq knob
		if (knob_toggle == AMP){
			knob_toggle = FREQ;
		}
		else if (knob_toggle == FREQ){
			knob_toggle = AMP;
		}
	}
	if (btnStateMachine(Shape) == true){
		//enqueue to change the wave shape
		//wavemsg[WAVE] = INC;
		//WaveQueue->enqueue(wavemsg); //enqueue to inc wave shape
	}
	if (btnStateMachine(Channel) == true){
		//toggle between channels
		if (chan_toggle == C1){
			chan_toggle = C2;
		}
		else if (chan_toggle == C2){
			chan_toggle = C1;
		}
	}
	if (btnStateMachine(Tshift) == true){
		//toggle follow mode
		if (follow_toggle == OFF){
			follow_toggle = ON;
		}
		else if (follow_toggle == ON){
			follow_toggle = OFF;
		}
	}
	if (knobStateMachine(htim1) == INC){
		if (knob_toggle == AMP){
			//wavemsg[AMP] = INC;
			//WaveQueue->enqueue(wavemsg);//enqueue to inc amplitude
		}
		else if (knob_toggle == FREQ){
			//wavemsg[FREQ] = INC;
			//WaveQueue->enqueue(wavemsg);//enqueue to inc frequency
		}
	}
	else if (knobStateMachine(htim2) == DEC){
		if (knob_toggle == AMP){
			//wavemsg[AMP] = DEC;
			//WaveQueue->enqueue(wavemsg);//enqueue to dec amplitude
			}
		else if (knob_toggle == FREQ){
			//wavemsg[FREQ] = DEC;
			//WaveQueue->enqueue(wavemsg);//enqueue to dec frequency
		}
	}
}

//////////State machine for Buttons//////////
InputDriver::btnStateMachine(Button btn){
	static int state = 0;
	switch(state){
	case 0:
		//button is idle and not being pressed
		if (btn.state()==0 && btn.state()!=btnLastState){//button is pressed check for bounce
			btnLastState=btn.state();
			state=1;
			return false;
		}
		else{ //button is still idle
			state=0;
			return false;
		}
		break;
	case 1:
		//button pressed down, check for bounce
		if (btn.state()==btnLastState){//button was in fact pressed down
			btnLastState=btn.state();
			state=2;
			return true;
		}
		else{ //button was not pressed down
			btnLastState=btn.state();
			state=0;
			return false;
		}
		break;
	case 2:
		//button is pressed down, waiting for release
		if (btn.state()==1 && btn.state()!=btnLastState){//button was released, check for bounce
			btnLastState=btn.state();
			state=0;
			return false;
		}
		else{ // button was not released
			btnLastState=btn.state();
			state=1;
			return false;
		}
	default:
			return false;
		break;
	}
}

//////////State Machine for Buttons//////////
InputDriver::knobStateMachine(TIM_HandleTypeDef *htim){

	tim_cnt = __HAL_TIM_GET_COUNTER(htim);

	if (tim_cnt > last_cnt){
		last_cnt = tim_cnt;
		return INC;
	}
	else if (tim_cnt< last_cnt){
		last_cnt = tim_cnt;
		return DEC;
	}
	else{
		return NA;
	}
}
