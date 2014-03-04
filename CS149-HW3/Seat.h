//
//  Seat.h
//  CS149-HW3
//
//  Created by Luca Severini on 3/1/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#ifndef __CS149_HW3__Seat__
#define __CS149_HW3__Seat__

#include <string>

using namespace std;

class Customer;
class Seller;

class Seat
{
public:
	Seat(char row, int col);
	~Seat();

private:
	char row;
	int col;
public:
	string name;
	Customer* customer;
	Seller* seller;
};

#endif /* defined(__CS149_HW3__Seat__) */
