/*
 * queue.h
 *
 *  Created on: Oct 22, 2024
 *      Author: Jmyer
 */

#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include "main.h"
#include <iostream>
#define Q_SIZE 20

s
class Queue{
public:
	void queueConstruct();
	bool enqueue(uint8_t type, int32_t value);
	bool dequeue(uint8_t type, int32_t& value);
private:
	int32_t buffer[Q_SIZE][5];
	int32_t tail=0;
};



#endif /* SRC_QUEUE_H_ */
