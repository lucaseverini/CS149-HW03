//
//  Seller.h
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

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
	deque<Customer>* customers;
	pthread_mutex_t *waitMutex;
	pthread_cond_t *waitCondition;
};

#endif /* defined(__CS149_HW3__Seller__) */
