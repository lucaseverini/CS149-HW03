//
//  Theatre.h
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#ifndef __CS149_HW3__Theatre__
#define __CS149_HW3__Theatre__

#include <pthread.h>
#include <deque>

using namespace std;

class Seat;
class Customer;

class Theatre
{
public:
	Theatre(int seatRows, int seatCols);
	~Theatre();

	void addCustomerToQueue(Customer& customer);
	void removeCustomerFromQueue(Customer& customer);
	Customer* getNextCustomerFromQueue(int customerType);
	Seat* assignSeatToCustomer(Customer* customer);
	bool soldOut();
	void addSeller();
	void removeSeller();
	void printSeats();

	int activeSellers;
	int availableSeats;
	int soldSeats;
	
private:
	void sendCustomersAway();
	
	int rows;
	int cols;
	Seat* *seats;
	deque<Customer*> queueH;
	deque<Customer*> queueM;
	deque<Customer*> queueL;
	int seatHRow;
	int seatHCol;
	int seatMRow;
	int seatMRowDec;
	int seatMCol;
	int seatLRow;
	int seatLCol;
public:
	pthread_mutex_t queueHMutex;
	pthread_cond_t queueHCondition;
	pthread_mutex_t queueMMutex;
	pthread_cond_t queueMCondition;
	pthread_mutex_t queueLMutex;
	pthread_cond_t queueLCondition;
	pthread_mutex_t seatMutex;
	pthread_mutex_t sellerMutex;
};

#endif /* defined(__CS149_HW3__Theatre__) */
