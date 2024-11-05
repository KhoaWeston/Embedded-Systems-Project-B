/*
 * InputDriver2.h
 *
 *  Created on: Oct 1, 2024
 *      Author: kcmartin
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_

#include "main.h"
#include "Button.h"
#include "Queue.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {A,F} AmpFreq;
typedef enum {C1,C2} Channel;
typedef enum {OFF,ON} Follow;
typedef enum {NA,INC,DEC} Moves;


class InputDriver{
private:
	//Queue *Wave_q;
	//Queue *Display_q;
	Moves msg;
	inputType type;
	AmpFreq knob_toggle;
	Channel chan_toggle;
	Follow follow_toggle;
	GPIO_PinState btnLastState;
	volatile uint32_t tim_cnt1;
	volatile uint32_t last_cnt1;
	volatile uint32_t tim_cnt2;
	volatile uint32_t last_cnt2;
public:
	/**
	 	* @brief constructor for InputDriver class.
	 	* initializes variables to be used within class functions.
	 	* @param x void accepts no parameters
	 	* @return void.
	 	*/
	InputDriver(void);
	/**
		* @brief Updates all potential inputs via calling state machines.
		* Enqueues the data collected by GPIO to send to output drivers.
	 	* @param x AmpFreq object of class "Button" input to "btnStateMachine" fnc.
	 	* @param x Shape object of class "Button" input to "btnStateMachine" fnc.
	 	* @param x Channel object of class "Button" input to "btnStateMachine" fnc.
	 	* @param x Follow object of class "Button" input to "btnStateMachine" fnc.
	 	* @param x htim1 pointer to htim1, input to "knobStateMachine".
	 	* @param x htim2 pointer to htim2, input to "knobStateMachine".
	 	* @return void.
	 	*/
	void update(Button AmpFreq ,Button Shape, Button Channel, Button Follow, TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2);
	/**
	 	 * @brief state machine to debounce button inputs.
	 	 * @param x btn object of class "Button"
	 	 * @return false if no press detected, true if press is detected
	 	 */
	bool btnStateMachine(Button btn);
	/**
		 	 * @brief compares new timer count to previous timer count
		 	 * to determine CW or CCW movement of knob 1.
		 	 * @param x *htim A pointer to a timer handle name
		 	 * @return INC if CW, DEC if CCW
		 	 */
	Moves knob1StateMachine(TIM_HandleTypeDef* htim);
	/**
			 * @brief compares new timer count to previous timer count
			 * to determine CW or CCW movement of knob 2.
			 * @param x *htim A pointer to a timer handle name
			 * @return INC if CW, DEC if CCW
			 */
	Moves knob2StateMachine(TIM_HandleTypeDef* htim);
};



#endif /* EMBEDDED_SYSTEMS_PROJECT_B_INPUTDRIVER_H_ */
