/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            Output.cpp
 * Purpose:         Allows output to be streamlined into one function
 * Start date:      3/6/14
 * Programmer:      Luca Severini, Romeo Stevens, Arash Zahoory
 *
 ******************************************************
 */

#ifndef CS149_HW3_Output_h
#define CS149_HW3_Output_h

#include <pthread.h>
#include <iostream>
#include "main.h"

/*-----------------------------------------------*/
/* PURPOSE:  Gathers data to be output, then outputs it either to shell or text file
 RECEIVES:   format: A string to be output, including variable number of parameters all being same type as format: const char*
 REMARKS: Allows a variable number of parameters not known at initialization of program.  Uses va_list to manage additional input.
 */
void output(const bool printTime, const char* format, ...)
{
	static pthread_mutex_t gOutputMutex = PTHREAD_MUTEX_INITIALIZER;
	
    //locking mutex to prevent critical region conflict between different output calls
	pthread_mutex_lock(&gOutputMutex);
	
	va_list vargs;
	
    /*allows subsequent calls to va_arg to sequentially retrieve the additional arguments passed to the function*/
	va_start(vargs, format);
	
	if(printTime)
	{
		fprintf(stdout, "%02d:%02d = ", elapsedSeconds / 60, elapsedSeconds % 60);
	}
	
	vfprintf(stdout, format, vargs);
	
	fflush(stdout);
	
	va_end(vargs);
	
	pthread_mutex_unlock(&gOutputMutex);
}

#endif
