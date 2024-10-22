/*
 * queue.cpp
 *
 *  Created on: Oct 22, 2024
 *      Author: Jmyer
 */

#include "Queue.h"


Queue::queueConstruct(){

}

Queue::enqueue(){
	if(tail<Q_SIZE){
		buffer[tail] = value;
		tail++;
		return true;
	}
	else{
		return false;
	}
}

Queue::dequeue(){
	if(buffer[0] != 0){
		value = buffer[0];
		for(int8_t i=0;i<Q_SIZE-1;i++){
			buffer[i] = buffer[i+1];
		}
		tail--1;
		return true;
	}
	else{
		return false;
	}
}
