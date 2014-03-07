/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            main.cpp
 * Purpose:         main class
 * Start date:      3/6/14
 * Programmer:      Luca Severini
 *
 ******************************************************
*/

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
/*----------------------------------------------------------*/
/* Main entry point to the program.  Customer and Seller threads are created here
 * Control is then given to the sellers, who sell tickets until appropriate conditions 
 * are met
 */
int main(int argc, const char * argv[])
{
	output("CS149 Assignment 3\n");
	output("March-4-2014\n");
	output("Team: Kamikaze\n\n");
    
	vector<Seller*> sellers;
	vector<Customer*> customers;
	int customersForSeller = 15;
    int runTime = 30;
	int seatCols = 10;
	int seatRows = 10;
	int maxWaitTime = 270;
	
	int totSeats = 0;
	int totCustomers = 0;
	
    //Taking in shell input
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
		else if(idx == 5)
		{
			maxWaitTime = atoi(argv[idx]);
		}
	}
	
	totSeats = seatCols * seatRows;
	totCustomers = customersForSeller * 10;
    
	output("Theatre with %d seats (%dx%d)\n", totSeats, seatCols, seatRows);
	output("%d customer(s) for seller\n", customersForSeller);
	output("Total customers: %d\n", totCustomers);
	output("Max. customer waiting in line: %d second(s)\n", maxWaitTime);
	output("Run-time %d second(s)\n", runTime);
    
	srand((unsigned int)time(NULL));
	
    //Initializing the theatre
	theatre = new Theatre(seatRows, seatCols);
    
	theatre->printSeats();
    
    //Creating Sellers
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
    
    //Creating customer threads for High Priority Sellers
	for(int idx = 1; idx <= customersForSeller; idx++)
	{
		customers.push_back(new Customer(1, idx, maxWaitTime));//priority, index, time
	}

    //Creating customer threads for Medium Priority Sellers
	for(int idx = 1; idx <= customersForSeller * 3; idx++)
	{
		customers.push_back(new Customer(2, idx, maxWaitTime));
	}
    
    //Creating customer threads for Low Priority Sellers
	for(int idx = 1; idx <= customersForSeller * 6; idx++)
	{
		customers.push_back(new Customer(3, idx, maxWaitTime));
	}
    
	output("Sale started\n");
    
    //Stop program if all seats are sold, or no customers left
	for(int idx = runTime; idx > 0; idx--)
	{
		output("%d second(s) to the end...\n", idx);
		
		sleep(1);
		
		if(theatre->activeSellers == 0)
		{
			output("All seats have been sold\n");
            
			theatre->sellTerminated = true;
			theatre->sendWaitingCustomersAway();
			theatre->releaseSellers();
			break;
		}
        
		if(theatre->processedCustomers == totCustomers)
		{
			output("All expected customers have been queued\n");
            
			theatre->sellTerminated = true;
			theatre->sendWaitingCustomersAway();
			theatre->releaseSellers();
			break;
		}
	}
	
	output("Sale terminated\n");
	
	theatre->printSeats();
	
	output("Sold seats: %d\n", theatre->soldSeats);
	
    //Keep track of unhappy customers
	int unseatedCustomers = 0;
	int unhappyCustomers = 0;
	for(vector<Customer*>::const_iterator iter = customers.begin(); iter != customers.end(); iter++)
	{
		if((*iter)->seat == NULL)
		{
			if((*iter)->waitTime >= maxWaitTime)
			{
				unhappyCustomers++;
			}
			else
			{
				unseatedCustomers++;
			}
		}
	}
	output("Unseated customers: %d\n", unseatedCustomers);
	output("Customers who waited %d second(s) or more: %d\n", maxWaitTime, unhappyCustomers);
	
    //Clean up Seller iterator
	for(vector<Seller*>::const_reverse_iterator iter = sellers.crbegin(); iter != sellers.crend(); iter++)
	{
		delete *iter;
	}
    
    //Clean up Customer iterator
	for(vector<Customer*>::const_reverse_iterator iter = customers.crbegin(); iter != customers.crend(); iter++)
	{
		delete *iter;
	}
    
	delete theatre;
	
	output("Done.\n");
    
    return 0;
}

