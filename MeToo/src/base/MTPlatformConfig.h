/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTPlatformConfig.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTPLATFORMCONFIG_H_  
#define MTPLATFORMCONFIG_H_  
  
/// @cond DO_NOT_SHOW

/**
Config of MeToo project, per target platform.
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which MT uses.
#define MT_PLATFORM_UNKNOWN            0
#define MT_PLATFORM_IOS                1
#define MT_PLATFORM_ANDROID            2
#define MT_PLATFORM_WIN32              3
#define MT_PLATFORM_MARMALADE          4
#define MT_PLATFORM_LINUX              5
#define MT_PLATFORM_BADA               6
#define MT_PLATFORM_BLACKBERRY         7
#define MT_PLATFORM_MAC                8
#define MT_PLATFORM_NACL               9
#define MT_PLATFORM_EMSCRIPTEN        10
#define MT_PLATFORM_TIZEN             11
#define MT_PLATFORM_QT5               12
#define MT_PLATFORM_WINRT             13

// Determine target platform by compile environment macro.
#define MT_TARGET_PLATFORM             MT_PLATFORM_UNKNOWN

// mac
#if defined(MT_TARGET_OS_MAC) || defined(__APPLE__)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_MAC
#endif

// iphone
#if defined(MT_TARGET_OS_IPHONE)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_IOS
#endif

// android
#if defined(ANDROID)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32) || defined(_WINDOWS)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_LINUX
#endif

// marmalade
#if defined(MARMALADE)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_MARMALADE
#endif

// bada
#if defined(SHP)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM         MT_PLATFORM_BADA
#endif

// qnx
#if defined(__QNX__)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM     MT_PLATFORM_BLACKBERRY
#endif

// native client
#if defined(__native_client__)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM     MT_PLATFORM_NACL
#endif

// Emscripten
#if defined(EMSCRIPTEN)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM     MT_PLATFORM_EMSCRIPTEN
#endif

// tizen
#if defined(TIZEN)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM     MT_PLATFORM_TIZEN
#endif

// qt5
#if defined(MT_TARGET_QT5)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM     MT_PLATFORM_QT5
#endif

// WinRT (Windows 8.1 Store/Phone App)
#if defined(WINRT)
#undef  MT_TARGET_PLATFORM
#define MT_TARGET_PLATFORM          MT_PLATFORM_WINRT
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if ! MT_TARGET_PLATFORM
#error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif 

#if (MT_TARGET_PLATFORM == MT_PLATFORM_WIN32)
#ifndef __MINGW32__
#pragma warning (disable:4127) 
#endif
#if defined(_MSC_VER)	// visual studio
#pragma warning(disable:4996) 
#endif
#endif  

/// @endcond
  
#endif // MTPLATFORMCONFIG_H_