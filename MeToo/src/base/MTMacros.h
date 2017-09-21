/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTMacros.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTMACROS_H_  
#define MTMACROS_H_  
  
#include "MTPlatformConfig.h"

// Generic macros

/// @name namespace metoo
#ifdef __cplusplus
#define NS_MT_BEGIN                     namespace metoo {
#define NS_MT_END                       }
#define USING_NS_MT                     using namespace metoo
#define NS_MT                           ::metoo
#else
#define NS_MT_BEGIN 
#define NS_MT_END 
#define USING_NS_MT 
#define NS_MT
#endif 

/// @name Metoo debug
#if defined(_DEBUG)
#define METOO_DEBUG 1
#else
#define METOO_DEBUG 0
#endif

#define __MTLOGWITHFUNCTION(s, ...) \
    log("%s : %s",__FUNCTION__, StringUtils::format(s, ##__VA_ARGS__).c_str())

#if !defined(METOO_DEBUG) || METOO_DEBUG == 0
#define MTLOG(...)       do {} while (0)
#define MTLOGINFO(...)   do {} while (0)
#define MTLOGERROR(...)  do {} while (0)
#define MTLOGWARN(...)   do {} while (0)

#elif METOO_DEBUG == 1
#define MTLOG(format, ...)      metoo::log(format, ##__VA_ARGS__)
#define MTLOGERROR(format,...)  metoo::log(format, ##__VA_ARGS__)
#define MTLOGINFO(format,...)   do {} while (0)
#define MTLOGWARN(...) __MTLOGWITHFUNCTION(__VA_ARGS__)

#elif METOO_DEBUG > 1
#define MTLOG(format, ...)      metoo::log(format, ##__VA_ARGS__)
#define MTLOGERROR(format,...)  metoo::log(format, ##__VA_ARGS__)
#define MTLOGINFO(format,...)   metoo::log(format, ##__VA_ARGS__)
#define MTLOGWARN(...) __MTLOGWITHFUNCTION(__VA_ARGS__)
#endif // METOO_DEBUG

/**
Helper macros which converts 4-byte little/big endian
integral number to the machine native number representation

It should work same as apples CFSwapInt32LittleToHost(..)
*/

/// when define returns true it means that our architecture uses big endian
#define MT_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define MT_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define MT_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define MT_SWAP_INT32_LITTLE_TO_HOST(i) ((MT_HOST_IS_BIG_ENDIAN == true)? MT_SWAP32(i) : (i) )
#define MT_SWAP_INT16_LITTLE_TO_HOST(i) ((MT_HOST_IS_BIG_ENDIAN == true)? MT_SWAP16(i) : (i) )
#define MT_SWAP_INT32_BIG_TO_HOST(i)    ((MT_HOST_IS_BIG_ENDIAN == true)? (i) : MT_SWAP32(i) )
#define MT_SWAP_INT16_BIG_TO_HOST(i)    ((MT_HOST_IS_BIG_ENDIAN == true)? (i):  MT_SWAP16(i) )


// new callbacks based on C++11
#define MT_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define MT_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define MT_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define MT_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)


/** @def MT_GETTER
* It is used to declare a protected variable. We can use getter to read the variable.
*
* @param varType     The type of variable.
* @param varName     Variable name.
* @param funName     "get + funName" will be the name of the getter.
* @warning   The getter is a public inline function.
*			 The variables and methods declared after MT_GETTER are all public.
*            If you need protected or private, please declare.
*/
#define MT_GETTER(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

#define MT_GETTER_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }

/** @def MT_SETTER
* It is used to declare a protected variable. We can use setter to set the variable.
*
* @param varType     The type of variable.
* @param varName     Variable name.
* @param funName     "set + funName" will be the name of the setter.
* @warning   The getter is a public inline function.
*			 The variables and methods declared after MT_SETTER are all public.
*            If you need protected or private, please declare.
*/
#define MT_SETTER(varType, varName, funName)\
protected: varType varName;\
public: virtual void set##funName(varType var) { varName = var; }

#define MT_SETTER_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual void set##funName(const varType& var) { varName = var; }


/** @def MT_GETTER_SETTER
* It is used to declare a protected variable.
* We can use getter to read the variable, and use the setter to change the variable.
*
* @param varType     The type of variable.
* @param varName     Variable name.
* @param funName     "get + funName" will be the name of the getter.
*                    "set + funName" will be the name of the setter.
* @warning   The getter and setter are public inline functions.
*            The variables and methods declared after MT_GETTER_SETTER are all public.
*            If you need protected or private, please declare.
*/
#define MT_GETTER_SETTER(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; }

#define MT_GETTER_SETTER_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var){ varName = var; }


#define MT_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define MT_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define MT_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)

  
#endif // MTMACROS_H_ 
