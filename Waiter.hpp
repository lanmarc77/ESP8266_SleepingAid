#ifndef WAITER_HPP
#define WAITER_HPP

class Waiter
{
	public:
		Waiter();
		
		void wait(unsigned int);
		
		bool isStillWaiting();
		
		void clear();
		
		unsigned int getRemainingTime();
		
	protected:
		unsigned int counter;
};

#endif
