

#ifndef EMBEDDED_SYSTEMS_PROJECT_B_CONTROLLER_H_
#define EMBEDDED_SYSTEMS_PROJECT_B_CONTROLLER_H_

#include "main.h"
#include "Channel.h"

class Controller{
private:
	Channel ch1;
	Channel ch2;
public:
	Controller(Channel*, Channel*);
	void update();
};

#endif
