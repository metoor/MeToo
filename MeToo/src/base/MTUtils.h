/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTUtils.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTUTILS_H_  
#define MTUTILS_H_  
  
#include "MTMacros.h"

#include <vector>
#include <string>

NS_MT_BEGIN

// namespace StringUtils
namespace StringUtils {
	const int MT_MAX_STRING_LENGTH = 1024 * 100;

	std::string format(const char* format, ...);

	//std::string &trim(std::string &s);
	std::string trim(const std::string &s);

	std::vector<std::string> split(const std::string &s, char delim);
}

NS_MT_END
  
#endif // MTUTILS_H_ 
