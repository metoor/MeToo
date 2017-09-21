/*************************************************
// Copyright (C), 2016-2020, CS&S. Co., Ltd.
// File name: 	MTSocket.cpp
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#include "MTSocket.h"

NS_MT_BEGIN

namespace net {

	//static variable 
	MTServerTCP* MTServerTCP::_server = nullptr;




	/************************************************************************/
	/*                           SocketMessage                              */
	/************************************************************************/
	SocketMessage::SocketMessage(MessageType type, unsigned char* data, int dataLen)
		: _msgType(type)
		, _errorType(ErrorType::NONE)
		, _msgData(nullptr)
	{
		_msgType = type;
		_msgData = new metoo::Data;
		_msgData->copy(data, dataLen);
	}

	SocketMessage::SocketMessage(MessageType type)
		: _msgType(type)
		, _msgData(nullptr)
		, _errorType(ErrorType::NONE)
	{
		if (MessageType::INCORRECT == type)
			_errorType = ErrorType::UNKNOW;
	}

	SocketMessage::SocketMessage(ErrorType type)
		: _msgType(MessageType::INCORRECT)
		, _msgData(nullptr)
		, _errorType(type)
	{
	}

	SocketMessage::~SocketMessage()
	{
		if (_msgData)
			MT_DELETE(_msgData);
	}

	/************************************************************************/
	/*                              MTSocket					   		  */
	/************************************************************************/
	MTSocket::MTSocket()
	{
		_isInitSuccess = false;
#if (MT_TARGET_PLATFORM == MT_PLATFORM_WIN32)
		WORD wVersionRequested;
		wVersionRequested = MAKEWORD(2, 0);
		WSADATA wsaData;
		int nRet = WSAStartup(wVersionRequested, &wsaData);
		if (nRet != 0)
		{
			MTLOG("Initilize socket Error!");
			return;
		}
		_isInitSuccess = true;
#endif
	}

	MTSocket::~MTSocket()
	{
#if (MT_TARGET_PLATFORM == MT_PLATFORM_WIN32)
		if (_isInitSuccess)
		{
			WSACleanup();
		}
#endif 
	}

	bool MTSocket::nonBlock(HSocket socket)
	{
#if (MT_TARGET_PLATFORM == MT_PLATFORM_ANDROID)

		int flags;
		flags = fcntl(socket, F_GETFL, 0);
		flags != O_NONBLOCK;
		if (fcntl(socket, F_SETFL, flags) < 0)
		{
			return false;
	}
#else
		u_long ulOn;
		ulOn = 1;
		if (ioctlsocket(socket, FIONBIO, &ulOn) == SOCKET_ERROR)
		{
			return false;
		}
#endif
		return true;
	}

	void MTSocket::closeConnect(HSocket socket)
	{
#if (MT_TARGET_PLATFORM == MT_PLATFORM_ANDROID)
		close(socket);
#elif (MT_TARGET_PLATFORM == MT_PLATFORM_WIN32)
		closesocket(socket);
#endif
	}

	bool MTSocket::IsError(HSocket socket)
	{
#if (MT_TARGET_PLATFORM == MT_PLATFORM_WIN32)
		return socket == SOCKET_ERROR;
#elif (MT_TARGET_PLATFORM == MT_PLATFORM_ANDROID)
		return socket < 0;
#endif
	}
	MTServerTCP * MTServerTCP::getInstance()
	{
		if (_server == nullptr)
		{
			_server = new MTServerTCP();
		}
		return _server;
	}
	void MTServerTCP::destroyInstance()
	{
		MT_DELETE(_server);
	}

	MTServerTCP::MTServerTCP()
	{
	}

	MTServerTCP::~MTServerTCP()
	{
	}
}

NS_MT_END