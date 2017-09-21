/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTCrossPlatformVar.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTCROSSPLATFORMVAR_H_  
#define MTCROSSPLATFORMVAR_H_  
  
#include "MTPlatformConfig.h"
#include <cstdint>

//define cross-platform base data
using uint = unsigned int;
using byte = std::uint8_t;

#ifdef _MSC_VER
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;

using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
#else

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
#endif
  
#endif // MTCROSSPLATFORMVAR_H_ 