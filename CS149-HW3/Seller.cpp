/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Seller.cpp
 * Purpose:         Seller class, provides context for seller threads
 * Start date:      3/6/14
 * Programmer:      Luca Severini, Romeo Stevens, Arash Zahoory
 *
 ******************************************************
 */

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

/* PURPOSE:  Seller main function
 RECEIVES:   context, recieved as void, but it is a pointer to a Seller object
 REMARKS: The Seller constructor utilizes this function as the procedure to follow when a Seller pthread is created, a seller mutex is used to prevent any critical region conflict between tickets that are being sold.
 */
void *Seller::main(void* context)
{
	Seller* _this = (Seller*)context;
	
	output("Start seller %s\n", _this->name.c_str());
	
	_this->setup();
	
	while(true)
	{
		pthread_mutex_lock(_this->waitMutex);
		pthread_cond_wait(_this->waitCondition, _this->waitMutex);//wait for customers
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

/*-----------------------------------------------*/
/* PURPOSE:  Constructor for Seller
 RECEIVES:   type: the type of Seller (high, medium, low priority), index: the 'identity' of the seller when multiple of same type exist.
 REMARKS: Creates a pthread for a Seller depending on the type of Seller
 */
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
}

//Clean up
Seller::~Seller()
{
	quit = true;
	pthread_join(this->threadId, NULL);

	// output("Seller %s deleted\n", name.c_str());
}

/*-----------------------------------------------*/
/* PURPOSE:  Simulates the sale of one ticket to a customer by specific seller
 RECEIVES:   A pointer to an instance of a customer object
 REMARKS: Creates a pthread for a Seller depending on the type of Seller
 */
void Seller::sellTicketToCustomer(Customer* customer)
{
    //A ticket is sold to a customer, allows seller to pause for appropriate representative time
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
		
        //provides wait condition for specific customer in line
		pthread_mutex_lock(&customer->waitMutex);
		pthread_cond_signal(&customer->waitCondition);
		pthread_mutex_unlock(&customer->waitMutex);
	}
}

/*-----------------------------------------------*/
/* PURPOSE:  Sets up the seller Mutexes based on the type of seller
 REMARKS: This is the last step that contributes to creating an instance of Seller
 */
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



