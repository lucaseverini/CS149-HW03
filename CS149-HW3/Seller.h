/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Seller.h
 * Purpose:         Header File for Seller.cpp
 * Start date:      3/6/14
 * Programmer:      Luca Severini, Romeo Stevens, Arash Zahoory
 *
 ******************************************************
 */

#ifndef __CS149_HW3__Seller__
#define __CS149_HW3__Seller__

#include <deque>
#include <pthread.h>
#include <string>

using namespace std;

class Customer;

class Seller
{
public:
	Seller(int type, int index);
	~Seller();

	void sellTicketToCustomer(Customer* customer);
	
private:
	static void *main(void* param);
	void setup();

public:
	pthread_t threadId;
	bool quit;
	int type;
	string name;
	bool sellPending;
	deque<Customer>* customers;
	pthread_mutex_t *waitMutex;
	pthread_cond_t *waitCondition;
};

#endif /* defined(__CS149_HW3__Seller__) */
