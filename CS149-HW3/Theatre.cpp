/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Theatre.cpp
 * Purpose:         Provides context for Seller and Customer Interaction
 * Start date:      3/6/14
 * Programmer:      Luca Severini, Romeo Stevens, Arash Zahoory
 *
 ******************************************************
 */

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

/* PURPOSE:  Constructor for Theatre
 RECEIVES:   seatRows: int, the number of rows that will be comprise the theatre.  seatCols: int, the number of columns that will comprise the theatre.
 REMARKS: initializes the pthread condition for waiting, initializes the mutex for waiting, creates the pthread responsible for the customer object referenced by this class. Starts procedure given by Customer's main class
 */
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
	seatLCol(0),
	processedCustomers(0),
	sellTerminated(false)
{
	seats = (Seat**)malloc(sizeof(Seat*) * (rows * cols));
	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			seats[INDEX(row, col)] = new Seat('A' + row, col);
		}
	}
    
    //initialize the mutex for high priority seller queue
	int result = pthread_mutex_init(&queueHMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueHMutex not initialized");
	}
    
    //set condition for mutex for high priority seller queue
	result = pthread_cond_init(&queueHCondition, NULL);
	if(result != 0)
	{
		perror("Seller::queueHCondition not initialized");
	}
    
    //initialize the mutex for medium priority seller queue
	result = pthread_mutex_init(&queueMMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueMMutex not initialized");
	}

    //set condition for mutex for medium priority seller queue
    result = pthread_cond_init(&queueMCondition, NULL);
	if(result != 0)
	{
		perror("Seller::queueMCondition not initialized");
	}

    //initialize the mutex for Low priority seller queue
	result = pthread_mutex_init(&queueLMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueLMutex not initialized");
	}
    
    //set condition for mutex for Low priority seller queue
	result = pthread_cond_init(&queueLCondition, NULL);
	if(result != 0)
	{
		perror("Seller::queueLCondition not initialized");
	}

    //initialize the mutex for seat
	result = pthread_mutex_init(&seatMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::queueLMutex not initialized");
	}

    //initialize the mutex for the seller
	result = pthread_mutex_init(&sellerMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::sellerMutex not initialized");
	}
    
    //initialize the mutex for the Customer
	result = pthread_mutex_init(&customersMutex, NULL);
	if(result != 0)
	{
		perror("Theatre::customersMutex not initialized");
	}
}

/*-----------------------------------------------*/
/* PURPOSE:  Clean up Theatre mutex/conditions
 */
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
	pthread_mutex_destroy(&customersMutex);

	for(int row = 0; row < rows; row++)
	{
		for(int col = 0; col < cols; col++)
		{
			delete seats[INDEX(row, col)];
		}
	}

	free(seats);
}

/*-----------------------------------------------*/
/* PURPOSE:  add's a customer to a seller's queue
 RECEIVES:  the address to a customer object
 REMARKS: Allows the customer to be added to queue based on customer priority
 */
void Theatre::addCustomerToQueue(Customer& customer)
{
    //Depending on customer priority, add to correct queue, broadcast mutex condition
	switch(customer.type)
	{
		case 1://high priority
			pthread_mutex_lock(&queueHMutex);
			
			queueH.push_back(&customer);

			output("Customer %s in queue H\n",customer.name.c_str());

			pthread_cond_broadcast(&queueHCondition);
			
			pthread_mutex_unlock(&queueHMutex);
			break;

		case 2://medium priority
			pthread_mutex_lock(&queueMMutex);
			
			queueM.push_back(&customer);

			output("Customer %s in queue M\n",customer.name.c_str());

			pthread_cond_broadcast(&queueMCondition);
			
			pthread_mutex_unlock(&queueMMutex);
			break;

		case 3://low priority
			pthread_mutex_lock(&queueLMutex);
			
			queueL.push_back(&customer);

			output("Customer %s in queue L\n",customer.name.c_str());

			pthread_cond_broadcast(&queueLCondition);
			
			pthread_mutex_unlock(&queueLMutex);
			break;
	}
}

/*-----------------------------------------------*/
/* PURPOSE:  removes a customer from its seller queue
 RECEIVES:  the address to a customer object
 REMARKS: Allows the customer to be added to queue based on customer priority
 */
void Theatre::removeCustomerFromQueue(Customer& customer)
{
    //Depending on customer type, remove from appropriate queue
	switch(customer.type)
	{
		case 1://high prioirty
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
            
		case 2://medium priority
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
            
		case 3://low prioirty
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
	
	pthread_mutex_lock(&customersMutex);
    
	//count number of customers processed
	processedCustomers++;
	
	pthread_mutex_unlock(&customersMutex);
}


/*-----------------------------------------------*/
/* PURPOSE:  Retrieves the next customer from a seller's queueu
 RECIEVES: the type of customer that is being retrieved from queue
 RETURNS:  A pointer to the customer that is being removed from the queue
 REMARKS: allows customers to be removed from seller queue's if they exist
 */
Customer* Theatre::getNextCustomerFromQueue(int customerType)
{
	Customer *customer = NULL;
	
	switch(customerType)
	{
		case 1://high priority
			pthread_mutex_lock(&queueHMutex);
			
			if(queueH.size() > 0)
			{
				customer = queueH.front();
				queueH.pop_front();

				pthread_mutex_lock(&customersMutex);
				processedCustomers++;
				pthread_mutex_unlock(&customersMutex);
			}
			else
			{
				output("Queue H is empty\n");
			}
			
			pthread_mutex_unlock(&queueHMutex);
			break;
			
		case 2://medium priority
			pthread_mutex_lock(&queueHMutex);

			if(queueM.size() > 0)
			{
				customer = queueM.front();
				queueM.pop_front();

				pthread_mutex_lock(&customersMutex);
				processedCustomers++;
				pthread_mutex_unlock(&customersMutex);
			}
			else
			{
				output("Queue M is empty\n");
			}

			pthread_mutex_unlock(&queueHMutex);
			break;
			
		case 3://low priority
			pthread_mutex_lock(&queueLMutex);

			if(queueL.size() > 0)
			{
				customer = queueL.front();
				queueL.pop_front();

				pthread_mutex_lock(&customersMutex);
				processedCustomers++;
				pthread_mutex_unlock(&customersMutex);
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

/*-----------------------------------------------*/
/* PURPOSE:  Changes state of the theatre, restricting sellers from selling tickets
 RETURNS:  whether or not the call is successful
 */
bool Theatre::soldOut()
{
	pthread_mutex_lock(&seatMutex);
	
	bool result = (availableSeats == 0);
	
	pthread_mutex_unlock(&seatMutex);
	
	return result;
}

/*-----------------------------------------------*/
/* PURPOSE:  assigns a customer object to a seat object
 RECEIVES: a pointer to a customer
 RETURNS:  the pointer to the seat that has been assigned
 REMARKS: This sets up the seating rows based on order recommended for priority seating
 */
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
		
	return assignedSeat;
}

/*-----------------------------------------------*/
/* PURPOSE: prints out the the arrangement of seats
 REMARKS: It converts the seating arrangement into a String stream, and uses the output function of the output class to print the seating arrangement
 */
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

/*-----------------------------------------------*/
/* PURPOSE:  Counts the number of active sellers
 */
void Theatre::addSeller()
{
	pthread_mutex_lock(&sellerMutex);
	
	activeSellers++;
	
	pthread_mutex_unlock(&sellerMutex);
}

/*-----------------------------------------------*/
/* PURPOSE:  Counts down the number of sellers being deactivated
 */
void Theatre::removeSeller()
{
	pthread_mutex_lock(&sellerMutex);
	
	activeSellers--;
	
	pthread_mutex_unlock(&sellerMutex);
}

/*-----------------------------------------------*/
/* PURPOSE:  instructs customers to leave their queues
 REMARKS: mutex condition to leave is set for every customer of every type
 */
void Theatre::sendWaitingCustomersAway()
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

/*-----------------------------------------------*/
/* PURPOSE:  instructs sellers to stop selling tickets
 REMARKS: mutex condition to stop is broadcasted to all seller types at the same time
 */
void Theatre::releaseSellers()
{
	output("Theatre sold out. Release all sellers...\n");
	
	pthread_mutex_lock(&queueHMutex);
	pthread_cond_broadcast(&queueHCondition);
	pthread_mutex_unlock(&queueHMutex);

	pthread_mutex_lock(&queueMMutex);
	pthread_cond_broadcast(&queueMCondition);
	pthread_mutex_unlock(&queueMMutex);

	pthread_mutex_lock(&queueLMutex);
	pthread_cond_broadcast(&queueLCondition);
	pthread_mutex_unlock(&queueLMutex);
}



