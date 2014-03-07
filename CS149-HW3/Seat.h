/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Seat.h
 * Purpose:         Header File for Seat.cpp
 * Start date:      3/6/14
 * Programmer:      Luca Severini
 *
 ******************************************************
 */

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
