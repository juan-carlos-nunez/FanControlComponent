/*
* File: log.h
*
* Description: Provides MACROS and other mechanisms for logging.
*
*/

#pragma once

#include <iostream>

#define PRINT_STD_OUT(str) std::cout << str << std::endl;

#ifdef _DEBUG
#define DEBUG_STD_OUT(str) std::cout << str << std::endl;
#else
#define DEBUG_STD_OUT(str) ;
#endif