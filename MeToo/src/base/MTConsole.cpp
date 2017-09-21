/*************************************************
// Copyright (C), 2016-2020, CS&S. Co., Ltd.
// File name: 	MTConsole.cpp
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#include "MTConsole.h"
#include <string>

#if MT_TARGET_PLATFORM==MT_PLATFORM_ANDROID
#include <android/log.h>
#endif 

NS_MT_BEGIN

static void _log(const char *format, va_list args)
{
	int bufferSize = MAX_LOG_LENGTH;
	char* buf = nullptr;

	do
	{
		buf = new (std::nothrow) char[bufferSize];
		if (buf == nullptr)
			return; // not enough memory

		int ret = vsnprintf(buf, bufferSize - 3, format, args);
		if (ret < 0)
		{
			bufferSize *= 2;

			delete[] buf;
		}
		else
			break;

	} while (true);

	strcat(buf, "\n");

#if MT_TARGET_PLATFORM == MT_PLATFORM_ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "metoo-x debug info", "%s", buf);
#else
	// win32, Linux, Mac, iOS, etc
	fprintf(stdout, "%s", buf);
	fflush(stdout);
#endif
	delete[] buf;
}

void log(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	_log(format, args);
	va_end(args);
}

void log(const std::string& str1)
{
	log("-string: %s", str1.c_str());
}

void log(const int& i)
{
	log("-int: %d", i);
}

void log(const bool& b)
{
	log("-bool: %s", b ? "true" : "false");
}

void log(const float& f)
{
	log("-float: %lf", f);
}

NS_MT_END