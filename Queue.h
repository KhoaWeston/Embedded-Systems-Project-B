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
#define Q_SIZE 10

class Queue{
public:
	void queueConstruct();
	bool enqueue(uint8_t type, uint32_t value);
	bool dequeue(uint8_t type, uint32_t& value);
private:
	uint32_t buffer[Q_SIZE][6];
	uint32_t tail=0;
	uint32_t head=0;
	enum inputType{FREQ,AMP,CHANNEL,FOLLOW,DELAY,WAVETYPE};
};



#endif /* SRC_QUEUE_H_ */
