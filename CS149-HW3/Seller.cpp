//
//  Seller.cpp
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#include <pthread.h>
#include <cstdlib>
#include <string>
#include <queue>
#include <unistd.h>
#include "main.h"
#include "Seller.h"
#include "Customer.h"
#include "Seat.h"
#include "Output.h"

using namespace std;

void *Seller::main(void* context)
{
	Seller* _this = (Seller*)context;
	
	output("Start seller %s\n", _this->name.c_str());
	
	_this->setup();
	
	while(true)
	{
		pthread_mutex_lock(_this->waitMutex);
		pthread_cond_wait(_this->waitCondition, _this->waitMutex);
		pthread_mutex_unlock(_this->waitMutex);
		
		Customer* customer = theatre->getNextCustomerFromQueue(_this->type);
		while(customer != NULL)
		{
			_this->sellTicketToCustomer(customer);
	
			customer = theatre->getNextCustomerFromQueue(_this->type);
		}
		
		if(theatre->soldOut() || theatre->sellTerminated)
		{
			_this->quit = true;
			break;
		}
	}

	theatre->removeSeller();

	output("End seller %s\n", _this->name.c_str());

	return NULL;
}

Seller::Seller(int type, int index)
:	type(type),
	quit(false),
	customers(NULL)
{
	char str[10];
	
	switch(type)
	{
		case 1:
			sprintf(str, "H%d", index);
			name = string(str);
			break;
			
		case 2:
			sprintf(str, "M%d", index);
			name = string(str);
			break;
			
		case 3:
			sprintf(str, "L%d", index);
			name = string(str);
			break;
	}

	int result = pthread_create(&this->threadId, NULL, Seller::main, this);
	if(result != 0)
	{
		perror("Seller::pthread_create not created");
	}

	// output("Seller %s created\n", name.c_str());
}

Seller::~Seller()
{
	quit = true;
	pthread_join(this->threadId, NULL);

	// output("Seller %s deleted\n", name.c_str());
}

void Seller::sellTicketToCustomer(Customer* customer)
{
	Seat *assignedSeat = theatre->assignSeatToCustomer(customer);
	if(assignedSeat != NULL)
	{
		int sleepVal = 0;
		
		switch(type)
		{
			case 1:
				sleepVal = (rand() % 2) + 1;
				break;

			case 2:
				sleepVal = (rand() % 4) + 2;
				break;

			case 3:
				sleepVal = (rand() % 5) + 4;
				break;
		}
		
		assignedSeat->seller = this;
		customer->seat = assignedSeat;

		sleep(sleepVal);

		output("Seller %s sold ticket to customer %s in %d second(s)\n", name.c_str(), customer->name.c_str(), sleepVal);		
		output("Available seats: %d\n", theatre->availableSeats);
		
		pthread_mutex_lock(&customer->waitMutex);
		pthread_cond_signal(&customer->waitCondition);
		pthread_mutex_unlock(&customer->waitMutex);
	}
}

void Seller::setup()
{
	switch(type)
	{
		case 1:
			waitMutex = &theatre->queueHMutex;
			waitCondition = &theatre->queueHCondition;
			break;

		case 2:
			waitMutex = &theatre->queueMMutex;
			waitCondition = &theatre->queueMCondition;
			break;

		case 3:
			waitMutex = &theatre->queueLMutex;
			waitCondition = &theatre->queueLCondition;
			break;
	}
	
	theatre->addSeller();
}



