/*************************************************
// Copyright (C), 2016-2020, CS&S. Co., Ltd.
// File name: 	MTUtils.cpp
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#include "MTUtils.h"
#include <algorithm>
#include <functional>
#include <cctype>
#include <sstream>
#include <stdarg.h>

NS_MT_BEGIN

namespace StringUtils {

	std::string format(const char* format, ...)
	{
		std::string ret;

		va_list ap;
		va_start(ap, format);

		char* buf = (char*)malloc(MT_MAX_STRING_LENGTH);
		if (buf != nullptr)
		{
			vsnprintf(buf, MT_MAX_STRING_LENGTH, format, ap);
			ret = buf;
			free(buf);
		}
		va_end(ap);

		return ret;
	}

	// trim from start --note:will modify origin string
	std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end --note:will modify origin string
	std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends --note:will modify origin string
	std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	//trim from both ends --note:can not modify origin string
	std::string trim(const std::string &s){
		std::string temp(s);
		return ltrim(rtrim(temp));
	}

	//split string by delim char eg. split("1*2*3", '*'); it will return ["1","2","3"]
	std::vector<std::string> split(const std::string &s, char delim) {
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> elems;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

} // namespace StringUtils

NS_MT_END