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

bool semaphore::flagFlip(){
		if(flag==false){
			flag=true;
			return true;
		}
		else if(flag==true){
			flag=false;
			return true;
		}
		else{
			return false;
		}
}

