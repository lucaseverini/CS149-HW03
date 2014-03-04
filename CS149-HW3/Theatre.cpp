//
//  Theatre.cpp
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <pthread.h>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "Theatre.h"
#include "Seat.h"
#include "Customer.h"
#include "Seller.h"
#include "Output.h"

#define INDEX(x, y) x * this->cols + y

using namespace std;

Theatre::Theatre(int seatRows, int seatCols)
:	rows(seatRows),
	cols(seatCols),
	activeSellers(0),
	availableSeats(seatRows * seatCols),
	soldSeats(0),
	seatHRow(0),
	seatHCol(0),
	seatMRow((seatRows - 1) / 2),
	seatMRowDec(1),
	seatMCol(0),
	seatLRow(seatRows - 1),
	seatLCol(0)
{
	seats = (Seat**)malloc(sizeof(Seat*) * (rows * cols));
	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			seats[INDEX(row, col)] = new Seat('A' + row, col);
		}
	}

	int result = pthread_mutex_init(&queueHMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueHMutex not initialized");
	}

	result = pthread_cond_init(&queueHCondition, NULL);
	if(result != 0)
	{
		perror("Seller::queueHCondition not initialized");
	}

	result = pthread_mutex_init(&queueMMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueMMutex not initialized");
	}

	result = pthread_cond_init(&queueMCondition, NULL);
	if(result != 0)
	{
		perror("Seller::queueMCondition not initialized");
	}

	result = pthread_mutex_init(&queueLMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueLMutex not initialized");
	}
	
	result = pthread_cond_init(&queueLCondition, NULL);
	if(result != 0)
	{
		perror("Seller::queueLCondition not initialized");
	}

	result = pthread_mutex_init(&seatMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueLMutex not initialized");
	}

	result = pthread_mutex_init(&sellerMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::sellerMutex not initialized");
	}
}

Theatre::~Theatre()
{
	pthread_mutex_destroy(&queueHMutex);
	pthread_cond_destroy(&queueHCondition);
	pthread_mutex_destroy(&queueMMutex);
	pthread_cond_destroy(&queueMCondition);
	pthread_mutex_destroy(&queueLMutex);
	pthread_cond_destroy(&queueLCondition);
	pthread_mutex_destroy(&seatMutex);
	pthread_mutex_destroy(&sellerMutex);

	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			delete seats[INDEX(row, col)];
		}
	}

	free(seats);
}

void Theatre::addCustomerToQueue(Customer& customer)
{
	switch(customer.type)
	{
		case 1:
			pthread_mutex_lock(&queueHMutex);
			
			queueH.push_back(&customer);

			output("Customer %s in queue H\n",customer.name.c_str());

			pthread_cond_broadcast(&queueHCondition);
			
			pthread_mutex_unlock(&queueHMutex);
			break;

		case 2:
			pthread_mutex_lock(&queueMMutex);
			
			queueM.push_back(&customer);

			output("Customer %s in queue M\n",customer.name.c_str());

			pthread_cond_broadcast(&queueMCondition);
			
			pthread_mutex_unlock(&queueMMutex);
			break;

		case 3:
			pthread_mutex_lock(&queueLMutex);
			
			queueL.push_back(&customer);

			output("Customer %s in queue L\n",customer.name.c_str());

			pthread_cond_broadcast(&queueLCondition);
			
			pthread_mutex_unlock(&queueLMutex);
			break;
	}
}

void Theatre::removeCustomerFromQueue(Customer& customer)
{
	switch(customer.type)
	{
		case 1:
			pthread_mutex_lock(&queueHMutex);

			for(deque<Customer*>::iterator iter = queueH.begin(); iter != queueH.end(); iter++)
			{
				if((*iter)->threadId == customer.threadId)
				{
					queueH.erase(iter);
					
					output("Customer %s left queue H\n",customer.name.c_str());
					break;
				}
			}
			
			pthread_mutex_unlock(&queueHMutex);
			break;
			
		case 2:
			pthread_mutex_lock(&queueHMutex);

			for(deque<Customer*>::iterator iter = queueM.begin(); iter != queueM.end(); iter++)
			{
				if((*iter)->threadId == customer.threadId)
				{
					queueM.erase(iter);
										
					output("Customer %s left queue M\n",customer.name.c_str());
					break;
				}
			}

			pthread_mutex_unlock(&queueHMutex);
			break;
			
		case 3:
			pthread_mutex_lock(&queueLMutex);

			for(deque<Customer*>::iterator iter = queueL.begin(); iter != queueL.end(); iter++)
			{
				if((*iter)->threadId == customer.threadId)
				{
					queueL.erase(iter);
					
					output("Customer %s left queue L\n",customer.name.c_str());
					break;
				}
			}

			pthread_mutex_unlock(&queueLMutex);
			break;
	}
}

Customer* Theatre::getNextCustomerFromQueue(int customerType)
{
	Customer *customer = NULL;
	
	switch(customerType)
	{
		case 1:
			pthread_mutex_lock(&queueHMutex);
			
			if(queueH.size() > 0)
			{
				customer = queueH.front();
				queueH.pop_front();
			}
			else
			{
				output("Queue H is empty\n");
			}
			
			pthread_mutex_unlock(&queueHMutex);
			break;
			
		case 2:
			pthread_mutex_lock(&queueHMutex);

			if(queueM.size() > 0)
			{
				customer = queueM.front();
				queueM.pop_front();
			}
			else
			{
				output("Queue M is empty\n");
			}

			pthread_mutex_unlock(&queueHMutex);
			break;
			
		case 3:
			pthread_mutex_lock(&queueLMutex);

			if(queueL.size() > 0)
			{
				customer = queueL.front();
				queueL.pop_front();
			}
			else
			{
				output("Queue L is empty\n");
			}

			pthread_mutex_unlock(&queueLMutex);
			break;
	}
	
	return customer;
}

bool Theatre::soldOut()
{
	pthread_mutex_lock(&seatMutex);
	
	bool result = (availableSeats == 0);
	
	pthread_mutex_unlock(&seatMutex);
	
	return result;
}

Seat* Theatre::assignSeatToCustomer(Customer* customer)
{
	Seat *assignedSeat = NULL;
	
	pthread_mutex_lock(&seatMutex);
	
	if(availableSeats != 0)
	{
		switch(customer->type)
		{
			// H rows
			case 1:
			{
				do
				{
					assignedSeat = seats[INDEX(seatHRow, seatHCol)];
				
					if(++seatHCol == cols)
					{
						seatHCol = 0;
						if(++seatHRow == rows)
						{
							break;
						}
					}
				}
				while(assignedSeat == NULL || assignedSeat->customer != NULL);
	
				if(assignedSeat != NULL)
				{
					assignedSeat->customer = customer;

					soldSeats++;
					availableSeats--;
				}
			}
				break;
				
			// M rows
			case 2:
			{
				do
				{
					assignedSeat = seats[INDEX(seatMRow, seatMCol)];
				
					if(++seatMCol == cols)
					{
						seatMCol = 0;
						
						seatMRow -= seatMRowDec % 2 == 0 ? seatMRowDec : -seatMRowDec;
						if(seatMRow < 0)
						{
							break;
						}

						seatMRowDec++;
					}
				}
				while(assignedSeat == NULL || assignedSeat->customer != NULL);
				
				if(assignedSeat != NULL)
				{
					assignedSeat->customer = customer;
					
					soldSeats++;
					availableSeats--;
				}
			}
				break;
				
			// L rows
			case 3:
			{
				do
				{
					assignedSeat = seats[INDEX(seatLRow, seatLCol)];
					
					if(++seatLCol == cols)
					{
						seatLCol = 0;
						if(--seatLRow == -1)
						{
							break;
						}
					}
				}
				while(assignedSeat == NULL || assignedSeat->customer != NULL);
				
				if(assignedSeat != NULL)
				{
					assignedSeat->customer = customer;
					
					soldSeats++;
					availableSeats--;
				}
			}
				break;
		}
	}
	   
	pthread_mutex_unlock(&seatMutex);
	
	if(availableSeats == 0)
	{
		sendCustomersAway();
	}
	
	return assignedSeat;
}

void Theatre::printSeats()
{
	stringstream outStream;
	
	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			Seat *seat = seats[INDEX(row, col)];
			outStream << seat->name << " [" << (seat->customer != NULL ? seat->customer->name : "----") << "/" << (seat->seller != NULL ? seat->seller->name : "--") << "]  ";
		}
		
		outStream << endl;
	}
	
	output("%s", outStream.str().c_str());
}

void Theatre::addSeller()
{
	pthread_mutex_lock(&sellerMutex);
	
	activeSellers++;
	
	pthread_mutex_unlock(&sellerMutex);
}

void Theatre::removeSeller()
{
	pthread_mutex_lock(&sellerMutex);
	
	activeSellers--;
	
	pthread_mutex_unlock(&sellerMutex);
}

void Theatre::sendCustomersAway()
{
	output("Theatre sold out. Send all waiting customers away...\n");

	for(deque<Customer*>::const_iterator iter = queueH.begin(); iter != queueH.end(); iter++)
	{
		pthread_mutex_lock(&(*iter)->waitMutex);
		pthread_cond_signal(&(*iter)->waitCondition);
		pthread_mutex_unlock(&(*iter)->waitMutex);
	}

	for(deque<Customer*>::const_iterator iter = queueM.begin(); iter != queueM.end(); iter++)
	{
		pthread_mutex_lock(&(*iter)->waitMutex);
		pthread_cond_signal(&(*iter)->waitCondition);
		pthread_mutex_unlock(&(*iter)->waitMutex);
	}

	for(deque<Customer*>::const_iterator iter = queueL.begin(); iter != queueL.end(); iter++)
	{
		pthread_mutex_lock(&(*iter)->waitMutex);
		pthread_cond_signal(&(*iter)->waitCondition);
		pthread_mutex_unlock(&(*iter)->waitMutex);
	}
}






