//
//  Output.cpp
//  CS149-HW3
//
//  Created by Luca Severini on 2/28/14.
//  Copyright (c) 2014 Luca Severini. All rights reserved.
//

#ifndef CS149_HW3_Output_h
#define CS149_HW3_Output_h

#include <pthread.h>
#include <iostream>

void output (const char* format, ...)
{
	static pthread_mutex_t gOutputMutex = PTHREAD_MUTEX_INITIALIZER;
	
	pthread_mutex_lock(&gOutputMutex);
	
	va_list vargs;
	
	va_start(vargs, format);
	vfprintf(stdout, format, vargs);
	
	fflush(stdout);
	
	va_end(vargs);
	
	pthread_mutex_unlock(&gOutputMutex);
}

#endif
