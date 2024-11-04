/*
 * semaphore.cpp
 *
 *  Created on: Oct 1, 2024
 *      Author: jmyers19
 */


#include "semaphore.h"

void semaphore::semaphoreConstruct(){
	flag = false;
}

bool semaphore::flagUp(uint32_t triggeringProcess){
		if(flag==false){
			flag=true;
			holder=triggeringProcess;
			return true;
		}
		else{
			return false;
		}
}

bool semaphore::flagDown(uint32_t triggeringProcess){
	if(holder==triggeringProcess){
		flag=false;
		holder=0;
		return true;
	}
	else{
		return false;
	}
}

