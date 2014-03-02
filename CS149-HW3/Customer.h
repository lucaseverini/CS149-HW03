//
//  Customer.h
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#ifndef __CS149_HW3__Customer__
#define __CS149_HW3__Customer__

#include <string>

using namespace std;

class Seat;

class Customer
{
public:
	Customer(int type, int index);
	~Customer();
		
	void wait(int time);
	void buyTicket();
	void leave();
	
private:
	static void *main(void* param);
	void sleepForAwhile();
	void goInQueue();
	void leaveQueue();

public:
	pthread_t threadId;
	int type;
	bool quit;
	string name;
	Seat* seat;
};

#endif /* defined(__CS149_HW3__Customer__) */
