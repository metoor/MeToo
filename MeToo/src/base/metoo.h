/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	metoo.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef METOO_H_  
#define METOO_H_  
  


// 0x00 HI ME LO
// 00   00 00 01
#define METOO_VERSION 0x00000001

#include "MTMacros.h"

//Output Debug message.
#include "MTConsole.h"

//include utils function.
#include "MTUtils.h"

//net lib
#include "../net/MTSocket.h"

NS_MT_BEGIN

const char* metooVersion();

NS_MT_END

  
#endif // METOO_H_ 