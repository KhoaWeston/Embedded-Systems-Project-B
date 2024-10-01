

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
