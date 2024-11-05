/*
 * queue.cpp
 *
 *  Created on: Oct 22, 2024
 *      Author: Jmyer
 */

#include "Queue.h"


void Queue::queueConstruct(){
	tail=0;
	for(uint8_t i=0;i<Q_SIZE; i++){
		for(uint8_t j=0;j<6; j++){
			buffer[i][j] = 0;
		}

	}
}

bool Queue::enqueue(uint8_t type, uint32_t value){
	if(buffer[tail][type]==0){
		if(tail<Q_SIZE){
			buffer[tail][type] = value;
			tail++;
			return true;
		}
		else if(tail==Q_SIZE){
			buffer[tail][type]=value;
			tail=0;
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

bool Queue::dequeue(uint8_t type, uint32_t& value){
	if(head<Q_SIZE){
		value = buffer[head][type];
		buffer[head][type]=0;
		head++;
		return true;
	}
	else if(head==Q_SIZE){
		value = buffer[head][type];
		buffer[head][type]=0;
		head=0;
		return true;
	}
	else{
		return false;
	}
}
