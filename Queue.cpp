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

Queue::enqueue(int32_t value){
	if(tail<Q_SIZE){
		buffer[tail] = value;
		tail++;
		return true;
	}
	else if(tail==Q_size){
		buffer[tail]=value;
		tail=0;
		return true;
	}
	else{
		return false;
	}
}

Queue::dequeue(int32_t& value){
	if(head>0){
		value = buffer[head];
		head--;
		return true;
	}
	else if(head ==0){
		value = buffer[head];
		head=Q_size;
	}
	else{
		return false;
	}
}
