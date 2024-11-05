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
	knob_toggle = A;
	chan_toggle = C1;
	follow_toggle = OFF;
	tim_cnt1 = 0;
	last_cnt1 = 0;
	tim_cnt2 = 0;
	last_cnt2 = 0;
	msg = NA;
	type = FREQ;
}

//////////Update the Driver, Send Data to Queues//////////

void InputDriver::update(Button AmpFreq ,Button Shape, Button Channel, Button Follow, TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2){
	if (btnStateMachine(AmpFreq) == true){
		//toggle the amp/freq knob
		if (knob_toggle == A){
			knob_toggle = F;
			GPIOA->ODR ^= 1<<11;//toggle debug
			//Display_q->enqueue(FREQ,knob_toggle);//enqueue FREQ to display
		}
		else if (knob_toggle == F){
			knob_toggle = A;
			GPIOA->ODR ^= 1<<11;//toggle debug
			//Display_q->enqueue(AMP,knob_toggle);//enqueue AMP to display
		}
	}
	if (btnStateMachine(Shape) == true){
		msg = INC;
		//Wave_q->enqueue(WAVETYPE,msg);//enqueue to inc wave shape
	}
	if (btnStateMachine(Channel) == true){
		//toggle between channels
		if (chan_toggle == C1){
			chan_toggle = C2;
			//Display_q->enqueue(CHANNEL, chan_toggle);
			//enqueue C2 to display
		}
		else if (chan_toggle == C2){
			chan_toggle = C1;
			//Display_q->enqueue(CHANNEL, chan_toggle);
			//enqueue C1 to display
		}
	}
	if (btnStateMachine(Follow) == true){
		//toggle follow mode
		if (follow_toggle == OFF){
			follow_toggle = ON;
			//Wave_q->enqueue(FOLLOW, follow_toggle);//enqueue follow ON to WAVE
			//Display_q->enqueue(FOLLOW, follow_toggle);//enqueue follow ON to display
		}
		else if (follow_toggle == ON){
			follow_toggle = OFF;
			//Wave_q->enqueue(FOLLOW, follow_toggle);//enqueue follow OFF to WAVE
			//Display_q->enqueue(FOLLOW, follow_toggle);//enqueue follow OFF to display

		}
	}
	//update the amp/frequency knob
	if (knob1StateMachine(htim1) == INC){
		if (knob_toggle == A){
			msg = INC;
			//Wave_q->enqueue(AMP,msg);//enqueue inc amplitude to WAVE
		}
		else if (knob_toggle == F){
			msg = INC;
			//Wave_q->enqueue(FREQ,msg);//enqueue inc frequency to WAVE
		}
	}
	else if (knob1StateMachine(htim1) == DEC){
		if (knob_toggle == A){
			msg = DEC;
			//Wave_q->enqueue(AMP,msg);//enqueue dec amplitude to WAVE
			}
		else if (knob_toggle == F){
			msg = DEC;
			//Wave_q->enqueue(FREQ,msg);//enqueue dec frequency to WAVE
		}
	}
	//update the time shift knob, only if follow==on
	if (follow_toggle == ON){
		if (knob2StateMachine(htim2) == INC){
			msg = INC;
			//Wave_q->enqueue(DELAY,msg);
			//enqueue INC time shift to WAVE
		}
		else if (knob2StateMachine(htim2) == DEC){
			msg = DEC;
			//Wave_q->enqueue(DELAY,msg);
			//enqueue DEC time shift to WAVE
		}
	}

}

//////////State machine for Buttons//////////
bool InputDriver::btnStateMachine(Button btn){
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
	case 1:
		//button pressed down, check for bounce
		if (btn.state()==btnLastState){//button was in fact pressed down
			btnLastState=btn.state();
			state=2;
			return true;
		}
		else{ //bounce detected
			btnLastState=btn.state();
			state=0;
			return false;
		}
	case 2:
		//button is pressed down, waiting for release
		if (btn.state()==1 && btn.state()!=btnLastState){//button was released, check for bounce
			btnLastState=btn.state();
			state=0;
			return false;
		}
		else{ // still being pressed
			btnLastState=btn.state();
			state=1;
			return false;
		}
	default:
			return false;
	}
}

//////////State Machine for Buttons//////////
Moves InputDriver::knob1StateMachine(TIM_HandleTypeDef *htim){

	tim_cnt1 = __HAL_TIM_GET_COUNTER(htim);

	if (tim_cnt1 > last_cnt1){
		last_cnt1 = tim_cnt1;
		return INC;
	}
	else if (tim_cnt1< last_cnt1){
		last_cnt1 = tim_cnt1;
		return DEC;
	}
	else{
		return NA;
	}
}

Moves InputDriver::knob2StateMachine(TIM_HandleTypeDef *htim){

	tim_cnt2 = __HAL_TIM_GET_COUNTER(htim);

	if (tim_cnt2 > last_cnt2){
		last_cnt2 = tim_cnt2;
		return INC;
	}
	else if (tim_cnt2< last_cnt2){
		last_cnt2 = tim_cnt2;
		return DEC;
	}
	else{
		return NA;
	}
}
