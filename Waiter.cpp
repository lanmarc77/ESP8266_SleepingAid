#include "Waiter.hpp"

Waiter::Waiter()
{
	counter = 0;
}
		
void Waiter::wait(unsigned int ms)
{
	counter = ms;
}

bool Waiter::isStillWaiting()
{
	if(counter > 0)
		--counter;
		
	return counter != 0;
}

void Waiter::clear()
{
	counter = 0;
}

unsigned int Waiter::getRemainingTime()
{
	return counter;
}
