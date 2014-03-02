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

using namespace std;

void *Customer::main(void* context)
{
	Customer* _this = (Customer*)context;
	
	output("Start customer %s\n", _this->name.c_str());
	
	_this->sleepForAwhile();
	
	_this->goInQueue();
	
	while(_this->seat == NULL && !_this->quit)
	{
		if(theatre->soldOut() && _this->seat == NULL)
		{
			_this->leaveQueue();
			break;
		}
		
		sleep(1);
	}
	
	output("End customer %s\n", _this->name.c_str());

	return NULL;
}

Customer::Customer(int type, int index)
:	type(type),
	quit(false),
	seat(NULL)
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

	int result = pthread_create(&this->threadId, NULL, Customer::main, this);
	if(result != 0)
	{
		perror("Customer::pthread_create not created");
	}
	
	output("Customer %s created\n", name.c_str());
}

Customer::~Customer()
{
	quit = true;
	pthread_join(this->threadId, NULL);
	
	output("Customer %s deleted\n", name.c_str());
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


