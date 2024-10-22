/*
 * queue.cpp
 *
 *  Created on: Oct 22, 2024
 *      Author: Jmyer
 */

#include "Queue.h"


Queue::queueConstruct(){
	tail=0;
	buffer=0;
}

Queue::enqueue(uint8_t type, int32_t value){
	/*Type is a number, 0-4 to represent the type of increment or decrement to be given to the queue.
	For example, 0=frequency, 1=amplitude, 2= Wave Shape, 3 = delay, 4=follow mode.
	The Value is set into the type section of the array to be pulled later. So long as this is consistently followed it should be fine
	*/
	if(tail<Q_SIZE){
		buffer[tail][type] = value;
		tail++;
		return true;
	}
	else{
		return false;
	}
}

Queue::dequeue(uint8_t type, int32_t& value){
	if(buffer[0] != 0){
		value = buffer[0][type];
		for(int8_t i=0;i<Q_SIZE-1;i++){
			buffer[i] = buffer[i+1];
		}
		tail-=1;
		return true;
	}
	else{
		return false;
	}
}
