


#include"main.h"

class Channel{
private:
	uint32_t freq;
	uint32_t amp;
	enum shape[sin,sq,pul];
	uint32_t delay;
public:
	Channel(freq, amp, shape, delay);
	void update();
}
