/******************************************************
 * Copyright (c):   2014 Luca Severini. All rights reserved.
 * Project:         CS 149 Hw3
 * File:            main.h
 * Purpose:         main header
 * Start date:      3/6/14
 * Programmer:      Luca Severini, Romeo Stevens, Arash Zahoory
 *
 ******************************************************
 */

#ifndef __CS149_HW3__main_h
#define __CS149_HW3__main_h

#include "Theatre.h"

extern Theatre* theatre;
extern int elapsedSeconds;

extern int clock_gettime(struct timespec *ts);

#endif /* defined(__CS149_HW3__main_h) */