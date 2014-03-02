//
//  main.cpp
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "Theatre.h"
#include "Customer.h"
#include "Seller.h"
#include "Output.h"

using namespace std;

Theatre* theatre;

int main(int argc, const char * argv[])
{
	vector<Seller*> sellers;
	vector<Customer*> customers;
	int customersForSeller = 5;		// 5, 10 or 15
	int runTime = 60;				// 60
	int totCustomers = 0;
	
	if(argc == 2)
	{
		customersForSeller = atoi(argv[1]);
	}

	output("CS149 Assignment 3\n");
	output("Team: Kamikaze\n");
	output("%d customer(s) for seller\n", customersForSeller);
	output("Runs for %d second(s)\n", runTime);

	srand((unsigned int)time(NULL));
	
	theatre = new Theatre(10, 10);
			
	theatre->printSeats();

	output("Creating Sellers...\n");
	sellers.push_back(new Seller(1, 1));
	sellers.push_back(new Seller(2, 1));
	sellers.push_back(new Seller(2, 2));
	sellers.push_back(new Seller(2, 3));
	sellers.push_back(new Seller(3, 1));
	sellers.push_back(new Seller(3, 2));
	sellers.push_back(new Seller(3, 3));
	sellers.push_back(new Seller(3, 4));
	sellers.push_back(new Seller(3, 5));
	sellers.push_back(new Seller(3, 6));

	totCustomers = customersForSeller * 10;
	output("Customers to seat: %d\n", totCustomers);

	for(int idx = 1; idx <= customersForSeller; idx++)
	{
		customers.push_back(new Customer(1, idx));
		totCustomers++;
	}
	
	for(int idx = 1; idx <= customersForSeller * 3; idx++)
	{
		customers.push_back(new Customer(2, idx));
		totCustomers++;
	}

	for(int idx = 1; idx <= customersForSeller * 6; idx++)
	{
		customers.push_back(new Customer(3, idx));
		totCustomers++;
	}

	sleep(runTime);

	theatre->printSeats();
	
	output("Unseated customers: %d\n", theatre->unseatedCustomers);
	
	for(vector<Seller*>::const_reverse_iterator iter = sellers.crbegin(); iter != sellers.crend(); iter++)
	{
		delete *iter;
	}

	for(vector<Customer*>::const_reverse_iterator iter = customers.crbegin(); iter != customers.crend(); iter++)
	{
		delete *iter;
	}

	delete theatre;
	
	sleep(3);
	
	output("Done.\n");

    return 0;
}

