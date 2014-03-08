/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Customer.h
 * Purpose:         Header for Customer Class
 * Start date:      3/6/14
 * Programmer:      Luca Severini, Romeo Stevens, Arash Zahoory
 *
 ******************************************************
 */

#ifndef __CS149_HW3__Customer__
#define __CS149_HW3__Customer__

#include <pthread.h>
#include <string>

using namespace std;

class Seat;

class Customer
{
public:
	Customer(int type, int index, int maxWaitTime = 0);
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
	int waitTime;
	int maxWaitTime;
	pthread_mutex_t waitMutex;
	pthread_cond_t waitCondition;
};

#endif /* defined(__CS149_HW3__Customer__) */
