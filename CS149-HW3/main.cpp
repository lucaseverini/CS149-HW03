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
	int customersForSeller = 70;
	int runTime = 300;
	int seatCols = 20;
	int seatRows = 30;
	int totSeats = 0;
	int totCustomers = 0;
	
	for(int idx = 1; idx < argc; idx++)
	{
		if(idx == 1)
		{
			customersForSeller = atoi(argv[idx]);
		}
		else if(idx == 2)
		{
			runTime = atoi(argv[idx]);
		}
		else if(idx == 3)
		{
			seatCols = atoi(argv[idx]);
		}
		else if(idx == 4)
		{
			seatRows = atoi(argv[idx]);
		}
	}
	
	totSeats = seatCols * seatRows;
	totCustomers = customersForSeller * 10;

	output("CS149 Assignment 3\n");
	output("March-2-2014\n");
	output("Team: Kamikaze\n\n");
	output("Theatre with %d seats (%dx%d)\n", totSeats, seatCols, seatRows);
	output("%d customer(s) for seller\n", customersForSeller);
	output("Total customers: %d\n", totCustomers);
	output("Run-time %d second(s)\n", runTime);

	srand((unsigned int)time(NULL));
	
	theatre = new Theatre(seatRows, seatCols);
			
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

	output("Sale started\n");

	for(int idx = runTime; idx > 0; idx--)
	{
		output("%d second(s) to the end...\n", idx);
		sleep(1);
		
		if(theatre->activeSellers == 0)
		{
			output("All seats have been sold\n");
			break;
		}
	}
	
	output("Sale terminated\n");
	
	theatre->printSeats();
	
	output("Sold seats: %d\n", theatre->soldSeats);
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
	
	output("Done.\n");

    return 0;
}

