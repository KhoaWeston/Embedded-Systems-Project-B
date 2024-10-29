/*
 * queue.cpp
 *
 *  Created on: Oct 22, 2024
 *      Author: Jmyer
 */

#include "Queue.h"


void Queue::queueConstruct(){
	tail=0;
	head=0;
	for(uint8_t i=0; i<Q_SIZE; i++){
		buffer[i]=0;
	}
}

bool Queue::enqueue(int32_t value){
	if(tail<Q_SIZE){
		buffer[tail] = value;
		tail++;
		return true;
	}
	else if(tail==Q_SIZE){
		buffer[tail]=value;
		tail=0;
		return true;
	}
	else{
		return false;
	}
}

bool Queue::dequeue(int32_t& value){
	if(head>0){
		value = buffer[head];
		head--;
		return true;
	}
	else if(head ==0){
		value = buffer[head];
		head=Q_SIZE;
		return true;
	}
	else{
		return false;
	}
}
