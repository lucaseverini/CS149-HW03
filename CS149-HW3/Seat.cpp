//
//  Seat.cpp
//  CS149-HW3
//
//  Created by Luca Severini on 3/1/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#include <iostream>
#include <string>
#include "Seat.h"
#include "Customer.h"
#include "Output.h"

using namespace std;

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

Seat::~Seat()
{
	output("Seat %s deleted\n", name.c_str());
}

void Seat::assign(Customer* customer)
{
}
