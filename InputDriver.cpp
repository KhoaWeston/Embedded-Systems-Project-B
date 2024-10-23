///*
// * InputDriver.cpp
// *
// *  Created on: Oct 1, 2024
// *      Author: kcmartin
// */
//
//#include "InputDriver.h"
//
//#define BTN_PRESSED GPIO_PIN_RESET //GPIO_PIN_RESET = 0
//#define BTN_DEPRESSED GPIO_PIN_SET //GPIO_PIN_SET = 1
//
//
//
//InputDriver::InputDriver(Queue* line){
//	queue = line;
//	next=OFF;
//	btnLastState=BTN_DEPRESSED;
//}
//
//
//InputDriver::btnAssign(Button btn){
//}
//
//InputDriver::btnStateMachine(Button btn){
//	static int state = 0;
//	switch(state){
//	case 0:
//		//button is idle and not being pressed
//		if (btn.state()==0 && btn.state()!=btnLastState){//button is pressed check for bounce
//			btnLastState=btn.state();
//			state=1;
//			return false;
//		}
//		else{ //button is still idle
//			state=0;
//			return false;
//		}
//		break;
//	case 1:
//		//button pressed down, check for bounce
//		if (btn.state()==btnLastState){//button was in fact pressed down
//			btnLastState=btn.state();
//			state=2;
//			return true;
//		}
//		else{ //button was not pressed down
//			btnLastState=btn.state();
//			state=0;
//			return false;
//		}
//		break;
//	case 2:
//		//button is pressed down, waiting for release
//		if (btn.state()==1 && btn.state()!=btnLastState){//button was released, check for bounce
//			btnLastState=btn.state();
//			state=0;
//			return false;
//		}
//		else{ // button was not released
//			btnLastState=btn.state();
//			state=1;
//			return false;
//		}
//	}
//}
