/*
 * queue.h
 *
 *  Created on: Oct 22, 2024
 *      Author: Jmyer
 */

#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_


/**
 * @brief Circular FIFO Queue
 */

#include "main.h"
#include <iostream>
#define Q_SIZE 10

class Queue{
public:
	/**
	 * @brief construct the Queue and fill the buffer array with 0's for initialization
	 */
	void queueConstruct();
	/**
	 * @brief writes a message (value) into the queue, and assigns what type of input it came from (type).
	 * This function utilizes the circular buffer via the head/tail style.
	 * The buffer's size is defined by the #DEFINE Q_SIZE earlier in this file, along with 6 different input types defined by the private enum
	 *
	 * @param x value an unsigned integer to store what input is to be stored to the FIFO buffer
	 * @param x type an unsigned integer to make what type of input is being stored for later use
	 * @return false if the enqueue is failed, true if successful
	 */
	bool enqueue(uint8_t type, uint32_t value);
	/**
	 * @brief stores the value at the data entry where the head is located and stores it into the variable value.
	 * Type here defines what type of input it is looking for.
	 * Once the data is extracted, the head moves up the queue to the next data entry to be extracted.
	 *
	 * @param x value an unsigned integer to collect what input was stored to the FIFO buffer
	 * @param x type an unsigned integer to make what type of input is being stored for later use
	 * @return false if the enqueue is failed, true if successful
	 */
	bool dequeue(uint8_t type, uint32_t& value);
private:
	uint32_t buffer[Q_SIZE][6];
	uint32_t tail=0;
	uint32_t head=0;
	enum inputType{FREQ,AMP,CHANNEL,FOLLOW,DELAY,WAVETYPE};
};



#endif /* SRC_QUEUE_H_ */
