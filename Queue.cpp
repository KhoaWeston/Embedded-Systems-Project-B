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

bool Queue::enqueue(int32_t type, int32_t value){
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

bool Queue::dequeue(int32_t type, int32_t& value){
	if(head>0){
		value = buffer[head][type];
		head--;
		return true;
	}
	else if(head ==0){
		value = buffer[head][type];
		head=Q_SIZE;
		return true;
	}
	else{
		return false;
	}
}
