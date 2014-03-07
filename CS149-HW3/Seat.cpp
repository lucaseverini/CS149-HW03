/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Seat.cpp
 * Purpose:         Represents seat objects
 * Start date:      3/6/14
 * Programmer:      Luca Severini
 *
 ******************************************************
 */

#include <iostream>
#include <string>
#include "Seat.h"
#include "Customer.h"
#include "Output.h"

using namespace std;
//Constructor for Seat class
Seat::Seat(char row, int col)
:	row(row),
	col(col),
	customer(NULL),
	seller(NULL)
{
	char str[10];
	sprintf(str, "%c%02d", row, col + 1);
	name = string(str);
}
//clean up
Seat::~Seat()
{
	// output("Seat %s deleted\n", name.c_str());
}
