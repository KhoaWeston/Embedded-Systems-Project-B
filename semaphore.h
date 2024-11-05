/*
 * semaphore.h
 *
 *  Created on: Oct 1, 2024
 *      Author: jmyers19
 */

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_SEMAPHORE_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_SEMAPHORE_H_

#include "main.h"
#include "Queue.h"
#include <iostream>

class semaphore{
public:
	/**
	* @brief construct the semaphore and initialize variables
	*/
	void semaphoreConstruct();
	/**
	 * @brief put the flag up and mark what process is holding it by it's process ID so only it can put the flag down
	 *
	 * @param x triggeringProcess an unsigned 32 bit integer marking what process is currently using the semaphore.
	 * @return true if the process grabbed the semaphore and is currently holding it, false if the flag is being held
	 */
	bool flagUp(uint32_t triggeringProcess);
	/**
	 * @brief the holding process let's go of the flag for the use of other processes by checking if it's processID matches with the holding ID
	 *
	 * @param x triggeringProcess an unsigned 32 bit integer to check if it matches with the holding id stored in holder.
	 * @return true if the process was successfully let go, false if the processID did not match the holder variable.
	 */
	bool flagDown(uint32_t triggeringProcess);
protected:
	bool flag;
	uint32_t holder;
};


#endif /* EMBEDDED_SYSTEMS_PROJECT_B_SEMAPHORE_H_ */
