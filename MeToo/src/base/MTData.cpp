/*************************************************
// Copyright (C), 2016-2020, CS&S. Co., Ltd.
// File name: 	MTData.cpp
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#include "MTData.h"
#include "MTConsole.h"

NS_MT_BEGIN

const Data Data::Null;

Data::Data() :
	_bytes(nullptr),
	_size(0)
{
	MTLOGINFO("In the empty constructor of Data.");
}

Data::Data(Data&& other) :
	_bytes(nullptr),
	_size(0)
{
	MTLOGINFO("In the move constructor of Data.");
	move(other);
}

Data::Data(const Data& other) :
	_bytes(nullptr),
	_size(0)
{
	MTLOGINFO("In the copy constructor of Data.");
	copy(other._bytes, other._size);
}

Data::~Data()
{
	MTLOGINFO("deallocing Data: %p", this);
	clear();
}

Data& Data::operator= (const Data& other)
{
	MTLOGINFO("In the copy assignment of Data.");
	copy(other._bytes, other._size);
	return *this;
}

Data& Data::operator= (Data&& other)
{
	MTLOGINFO("In the move assignment of Data.");
	move(other);
	return *this;
}

void Data::move(Data& other)
{
	_bytes = other._bytes;
	_size = other._size;

	other._bytes = nullptr;
	other._size = 0;
}

bool Data::isNull() const
{
	return (_bytes == nullptr || _size == 0);
}

unsigned char* Data::getBytes() const
{
	return _bytes;
}

std::size_t Data::getSize() const
{
	return _size;
}

void Data::copy(const unsigned char* bytes, const std::size_t size)
{
	clear();

	if (size > 0)
	{
		_size = size;
		_bytes = (unsigned char*)malloc(sizeof(unsigned char) * _size);
		memcpy(_bytes, bytes, _size);
	}
}

void Data::fastSet(unsigned char* bytes, const std::size_t size)
{
	_bytes = bytes;
	_size = size;
}

void Data::clear()
{
	free(_bytes);
	_bytes = nullptr;
	_size = 0;
}

NS_MT_END