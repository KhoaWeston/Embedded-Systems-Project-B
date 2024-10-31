/*
 * semaphore.h
 *
 *  Created on: Oct 1, 2024
 *      Author: jmyers19
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_SEMAPHORE_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_SEMAPHORE_H_

#include "main.h"
#include <iostream>

class semaphore{
public:
	void semaphoreConstruct();
	bool flagFlip();
private:
	bool flag;

};


#endif /* EMBEDDED_SYSTEMS_PROJECT_B_SEMAPHORE_H_ */
