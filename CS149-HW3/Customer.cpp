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
#include <pthread.h>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "Customer.h"
#include "Output.h"
#include "main.h"
#include <sys/errno.h>

using namespace std;

/* PURPOSE:  Customer main function
 RECEIVES:   context, recieved as void, but it is a pointer to a Customer object
 REMARKS: The Customer object's state is set according to circumstances set by Seller.  The Customer constructor utilizes this function as the procedure to follow when a Customer pthread is created.  A customer wait mutex is used to prevent multiple customers from interactingh with a single seller at the same time, and ensures the Customer stays at the head of its queue.
 */
void *Customer::main(void* context)
{
	Customer* _this = (Customer*)context;
	
	output(true, "Customer %s starts\n", _this->name.c_str());
	
	_this->sleepForAwhile();
	
	bool waitExpired = false;

	if(!theatre->soldOut())
	{
		//Customer is placed in line
		_this->goInQueue();
		output(true, "Customer %s is waiting in line...\n", _this->name.c_str());
		
		pthread_mutex_lock(&_this->waitMutex);
		
		time_t startTime = time(NULL);
		time_t curTime = time(NULL);
		
		//As long as the Customer has not waited too long, and there are tickets to buy
		//define customer state based on given circumstances
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
		
		curTime = time(NULL);//Current time is set for moment customer leaves line
		
		pthread_mutex_unlock(&_this->waitMutex);
		
		_this->waitTime = (int)(curTime - startTime);
    }
	
	if(_this->seat == NULL && theatre->soldOut())//Customer leaves if sold out
	{
		_this->quit = true;
		_this->leaveQueue();
		
		output(true, "Theatre is sold out and customer %s left\n", _this->name.c_str(), _this->waitTime);
	}
	else if(_this->seat == NULL && waitExpired)//Customer leaves if waited too long
	{
		_this->quit = true;
		_this->leaveQueue();
		
		output(true, "Customer %s waited %d seconds and left\n", _this->name.c_str(), _this->waitTime);
	}
	
	output(true, "Customer %s stops\n", _this->name.c_str());
    
	return NULL;
}

/*-----------------------------------------------*/
/* PURPOSE:  Constructor for Customer
 RECEIVES:   type: the type of Customer (high, medium, low priority), index: the 'identity' of the Customer when multiple of same type exist, maxWaitTime: the maximum allowed time customer is allowed to wait in time before leaving
 REMARKS: initializes the pthread condition for waiting, initializes the mutex for waiting, creates the pthread responsible for the customer object referenced by this class. Starts procedure given by Customer's main class
 */
Customer::Customer(int type, int index, int maxWaitTime)
:	type(type),
quit(false),
seat(NULL),
waitTime(0),
maxWaitTime(maxWaitTime)
{
	char str[10];
	//Create Customer based on type of parameter given,
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
    
    //set condition for pthread
	int result = pthread_cond_init(&waitCondition, NULL);
	if(result != 0)
	{
		perror("Customer::waitCondition not initialized");
	}
    
    //set condition for mutex
	result = pthread_mutex_init(&waitMutex, NULL);
	if(result != 0)
	{
		perror("Customer::waitMutex not initialized");
	}
    
    //create pthread
	result = pthread_create(&this->threadId, NULL, Customer::main, this);
	if(result != 0)
	{
		perror("Customer::pthread_create not created");
	}
}

/*-----------------------------------------------*/
//Clean up
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

/*-----------------------------------------------*/
/* PURPOSE:  Allows the Customer object to simulate arrival time by sleeping for random time
*/
void Customer::sleepForAwhile()
{
	int sleepVal = rand() % 59;
	output(true, "Customer %s sleeps/wait for %d seconds\n", name.c_str(), sleepVal);
	sleep(sleepVal);
}

/*-----------------------------------------------*/
/* PURPOSE:  Instructs the Customer object to be added to the theatre queue
 */
void Customer::goInQueue()
{
	theatre->addCustomerToQueue(*this);
}

/*-----------------------------------------------*/
/* PURPOSE:  Instructs the Customer object to leave the theatre queue
 */
void Customer::leaveQueue()
{
	theatre->removeCustomerFromQueue(*this);
}

