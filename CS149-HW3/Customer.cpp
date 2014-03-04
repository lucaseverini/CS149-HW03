//
//  Customer.cpp
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#include <pthread.h>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "Customer.h"
#include "Output.h"
#include "main.h"
#include <time.h>
#include <sys/time.h>
#include <sys/errno.h>

using namespace std;

void *Customer::main(void* context)
{
	Customer* _this = (Customer*)context;
	
	output("Start customer %s\n", _this->name.c_str());
	
	_this->sleepForAwhile();
	
	_this->goInQueue();
		
	output("Customer %s is waiting in line...\n", _this->name.c_str());
			
	pthread_mutex_lock(&_this->waitMutex);

	time_t startTime = time(NULL);
	time_t curTime = time(NULL);
	
	bool waitExpired = false;
	while(_this->seat == NULL && !waitExpired && !theatre->soldOut())
	{
		if(_this->maxWaitTime != 0)
		{
			struct timespec timeToWait;
			clock_gettime(&timeToWait);
			timeToWait.tv_sec += (long)_this->maxWaitTime;

			if(pthread_cond_timedwait(&_this->waitCondition, &_this->waitMutex, &timeToWait) == ETIMEDOUT)
			{
				waitExpired = true;
			}
		}
		else
		{
			pthread_cond_wait(&_this->waitCondition, &_this->waitMutex);
		}
	}

	curTime = time(NULL);

	pthread_mutex_unlock(&_this->waitMutex);

	_this->waitTime = (int)(curTime - startTime);

	if(_this->seat == NULL && theatre->soldOut())
	{
		_this->quit = true;
		_this->leaveQueue();
		
		output("Theatre is sold out and customer %s left\n", _this->name.c_str(), curTime - startTime);
	}
	else if(_this->seat == NULL && waitExpired)
	{
		_this->quit = true;
		_this->leaveQueue();
		
		output("Customer %s waited %d seconds and left\n", _this->name.c_str(), curTime - startTime);
	}
	
	output("End customer %s\n", _this->name.c_str());

	return NULL;
}

Customer::Customer(int type, int index, int maxWaitTime)
:	type(type),
	quit(false),
	seat(NULL),
	waitTime(0),
	maxWaitTime(maxWaitTime)
{
	char str[10];
	
	switch(type)
	{
		case 1:
			sprintf(str, "H%03d", index);
			name = string(str);
			break;
			
		case 2:
			sprintf(str, "M%03d", index);
			name = string(str);
			break;
			
		case 3:
			sprintf(str, "L%03d", index);
			name = string(str);
			break;
	}

	int result = pthread_cond_init(&waitCondition, NULL);
	if(result != 0)
	{
		perror("Customer::waitCondition not initialized");
	}

	result = pthread_mutex_init(&waitMutex, NULL);
	if(result != 0)
	{
		perror("Customer::waitMutex not initialized");
	}

	result = pthread_create(&this->threadId, NULL, Customer::main, this);
	if(result != 0)
	{
		perror("Customer::pthread_create not created");
	}
	
	// output("Customer %s created\n", name.c_str());
}

Customer::~Customer()
{
	quit = true;
	
	pthread_mutex_lock(&waitMutex);
	pthread_cond_signal(&waitCondition);
	pthread_mutex_unlock(&waitMutex);
	
	pthread_join(this->threadId, NULL);
	
	pthread_mutex_destroy(&waitMutex);
	pthread_cond_destroy(&waitCondition);
	
	// output("Customer %s deleted\n", name.c_str());
}

void Customer::sleepForAwhile()
{
	int sleepVal = rand() % 59;
	printf("Customer %s sleeps for %d seconds\n", name.c_str(), sleepVal);
	sleep(sleepVal);
}

void Customer::goInQueue()
{
	theatre->addCustomerToQueue(*this);
}

void Customer::leaveQueue()
{
	theatre->removeCustomerFromQueue(*this);
}

// OS X does not have clock_gettime so we simulate it with clock_get_time
static int clock_gettime(struct timespec *ts)
{
    struct timeval now;
	
    int result = gettimeofday(&now, NULL);
	if(result != 0)
	{
		return result;
	}
	
    ts->tv_sec = now.tv_sec;
	ts->tv_nsec = now.tv_usec * 1000;
	
    return 0;
}


