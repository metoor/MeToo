/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTConsole.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTCONSOLE_H_  
#define MTCONSOLE_H_  
  
#include "MTMacros.h"
#include <stdarg.h>
#include <cstddef>
#include <string>

NS_MT_BEGIN

/// The max length of CCLog message.
static const std::size_t MAX_LOG_LENGTH = 16 * 1024;

/**
@brief Output Debug message.
*/
void log(const char * format, ...);

/**
@brief Output type message.
*waring: float is conflict with math.h
*please use namespace --eg. metoo::log(1.5f); only float type
*/
void log(const std::string& str1);
void log(const int& i);
void log(const bool& b);
void log(const float& f);

NS_MT_END
  
#endif // MTCONSOLE_H_ 
