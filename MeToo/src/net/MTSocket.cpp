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

	bool MTSocket::ipv4Init(struct sockaddr_in& sockAddr, HSocket& socketfd, const unsigned short port, const std::string& ip, int type)
	{
		if (socketfd != INVALID_ST)
		{
			closeConnect(socketfd);
		}
		socketfd = socket(AF_INET, type, 0);
		if (IsError(socketfd)) 
		{
			socketfd = INVALID_ST;
			return false;
		}

		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port);
		sockAddr.sin_addr.s_addr = ip.empty() ? htonl(INADDR_ANY): inet_addr(ip.c_str());
		return true;
	}

	bool MTSocket::ipv6Init(struct sockaddr_in6& sockAddr, HSocket & socketfd, const unsigned short port, const std::string & ip, int type)
	{
		
		return true;
	}

	/************************************************************************/
	/*                              MTServerTCP					   		  */
	/************************************************************************/
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

	bool MTServerTCP::startServer(unsigned short port)
	{
		if (!initServer(port))
		{
			return false;
		}

		return true;
	}

	MTServerTCP::MTServerTCP()
		: _socketServer(INVALID_ST)
		, _serverPort(0)
		, _isRunning(false)
		, onStart(nullptr)
		, onRecv(nullptr)
		, onNewConnection(nullptr)
		, onDisconnect(nullptr)
	{
	}

	void MTServerTCP::clear()
	{
		if(_socketServer)
		{
			_mutex.lock();
			this->closeConnect(_socketServer);
			_mutex.unlock();
		}

		for (auto msg : _UIMessageQueue)
		{
			MT_DELETE(msg);
		}
		_UIMessageQueue.clear();
	}

	bool MTServerTCP::initServer(unsigned short port)
	{
		do{
			int ret = 0;
			struct sockaddr_in serverAddr;
			ret = ipv4Init(serverAddr, _socketServer, port);
			if (!ret)
			{
				_onError(ErrorType::INIT_FAILED, "socket init failed!");
				break;
			}

			ret = bind(_socketServer, (const sockaddr*)&serverAddr, sizeof(serverAddr));
			if (ret < 0)
			{
				_onError(ErrorType::BIND_FAILED, "socket bind failed!");
				break;
			}

			ret = listen(_socketServer, 5);
			if (ret < 0)
			{
				_onError(ErrorType::LISTEN_FAILED, "socket listen failed!");
				break;
			}

			// start 
			char hostName[256];
			gethostname(hostName, sizeof(hostName));
			struct hostent* hostInfo = gethostbyname(hostName);
			char* ip = inet_ntoa(*(struct in_addr *)*hostInfo->h_addr_list);
			
			this->acceptClient();

			if (onStart != nullptr)
			{
				log("start server!");
				this->onStart(ip, port);
			}

			return true;
		} while (false);

		closeConnect(_socketServer);
		_socketServer = 0;
		return false;
	}

	void MTServerTCP::acceptClient()
	{
		std::thread th(&MTServerTCP::acceptFunc, this);
		th.detach();
	}

	void MTServerTCP::acceptFunc()
	{
		int len = sizeof(sockaddr);
		struct sockaddr_in sockAddr;
		while (true)
		{
			HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, &len);
			if (IsError(clientSock))
			{
				MTLOG("socket error: accept error!");
			}
			else
			{
				this->newClientConnected(clientSock);
			}
		}
	}

	MTServerTCP::~MTServerTCP()
	{
		clear();
	}

	void MTServerTCP::_onError(ErrorType type, const std::string & des)
	{
		MTLOG("socket error:" + des);
	}
}

NS_MT_END