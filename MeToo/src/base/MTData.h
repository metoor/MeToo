/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTData.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTDATA_H_  
#define MTDATA_H_  
  
#include "MTMacros.h"
#include <stdint.h>
#include <string> 
#include <cstddef>
#include "MTCrossPlatformVar.h"

NS_MT_BEGIN

class Data
{
	friend class Properties;

public:
	/**
	* This parameter is defined for convenient reference if a null Data object is needed.
	*/
	static const Data Null;

	/**
	* Constructor of Data.
	*/
	Data();

	/**
	* Copy constructor of Data.
	*/
	Data(const Data& other);

	/**
	* Copy constructor of Data.
	*/
	Data(Data&& other);

	/**
	* Destructor of Data.
	*/
	~Data();

	/**
	* Overroads of operator=.
	*/
	Data& operator= (const Data& other);

	/**
	* Overroads of operator=.
	*/
	Data& operator= (Data&& other);

	/**
	* Gets internal bytes of Data. It will retrun the pointer directly used in Data, so don't delete it.
	*
	* @return Pointer of bytes used internal in Data.
	*/
	unsigned char* getBytes() const;

	/**
	* Gets the size of the bytes.
	*
	* @return The size of bytes of Data.
	*/
	std::size_t getSize() const;

	/** Copies the buffer pointer and its size.
	*  @note This method will copy the whole buffer.
	*        Developer should free the pointer after invoking this method.
	*  @see Data::fastSet
	*/
	void copy(const unsigned char* bytes, const std::size_t size);

	/** Fast set the buffer pointer and its size. Please use it carefully.
	*  @param bytes The buffer pointer, note that it have to be allocated by 'malloc' or 'calloc',
	*         since in the destructor of Data, the buffer will be deleted by 'free'.
	*  @note 1. This method will move the ownship of 'bytes'pointer to Data,
	*        2. The pointer should not be used outside after it was passed to this method.
	*  @see Data::copy
	*/
	void fastSet(unsigned char* bytes, const std::size_t size);

	/**
	* Clears data, free buffer and reset data size.
	*/
	void clear();

	/**
	* Check whether the data is null.
	*
	* @return True if the the Data is null, false if not.
	*/
	bool isNull() const;

private:
	void move(Data& other);

private:
	unsigned char* _bytes;
	std::size_t _size;
};


NS_MT_END
  
#endif // MTDATA_H_ 