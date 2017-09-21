/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTSocket.h
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#ifndef MTSOCKET_H_  
#define MTSOCKET_H_  
  
#include "../base/MTMacros.h"
#include "../base/MTData.h"
#include "../base/MTConsole.h"
#include <list>
#include <thread>
#include <mutex>

#if (MT_TARGET_PLATFORM == MT_PLATFORM_WIN32)
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
#define HSocket SOCKET

#elif (MT_TARGET_PLATFORM == MT_PLATFORM_ANDROID)
#include <error.h>
#include <arpa/inet.h>		// for inet_**
#include <netdb.h>			// for gethost**
#include <netinet/in.h>		// for sockaddr_in
#include <sys/types.h>		// for socket
#include <sys/socket.h>		// for socket
#include <unistd.h>
#include <stdio.h>		    // for printf
#include <stdlib.h>			// for exit
#include <string.h>			// for bzero
#define HSocket int
#endif 

NS_MT_BEGIN

namespace net {
	enum MessageType
	{
		DISCONNECT,
		RECEIVE,
		NEW_CONNECTION,
		INCORRECT,
	};

	enum ErrorType
	{
		NONE,
		INIT_FAILED,
		UNKNOW
	};

	/************************************************************************/
	/*                           SocketMessage                              */
	/************************************************************************/
	class SocketMessage
	{
	public:
		//constructor
		SocketMessage(MessageType type);
		SocketMessage(ErrorType type);
		SocketMessage(MessageType type, unsigned char* data, int dataLen);
		~SocketMessage();

		//getter
		inline const metoo::Data* getMsgData() { return _msgData; }
		inline const MessageType& getMsgType() { return _msgType; }
		inline const ErrorType& getErrorType() { return _errorType; }
	private:
		MessageType _msgType;
		ErrorType _errorType;
		metoo::Data* _msgData;
	};

	/************************************************************************/
	/*                              MTSocket					   		  */
	/************************************************************************/
	class MTSocket
	{
	public:
		MTSocket();
		virtual ~MTSocket();
		bool nonBlock(HSocket socket);

	protected:
		void closeConnect(HSocket socket);
		bool IsError(HSocket socket);

	protected:
		//virtual bool init() = 0;
		//virtual bool connect() = 0;

	protected:
		std::mutex _mutex;

	private:
		bool _isInitSuccess;
	};

	/************************************************************************/
	/*                              MTSocket					   		  */
	/************************************************************************/
	class MTServerTCP : public MTSocket
	{
	public:
		static MTServerTCP* getInstance();
		void destroyInstance();

		bool startServer(unsigned short port);
		void sendMessage(HSocket socket, const char* data, int count);
		void sendMessage(const char* data, int count);
		void update(float dt);

		std::function<void(const char* ip)> onStart;
		std::function<void(HSocket socket)> onNewConnection;
		std::function<void(HSocket socket, const char* data, int count)> onRecv;
		std::function<void(HSocket socket)> onDisconnect;

	protected:
		~MTServerTCP();

	private:
		MTServerTCP();
		void clear();
		bool initServer(unsigned short port);
		void acceptClient();
		void acceptFunc();
		void newClientConnected(HSocket socket);
		void recvMessage(HSocket socket);

	private:
		struct ReciveData
		{
			HSocket socketClient;
			Data* data;
		};

	private:
		static MTServerTCP* _server;
		HSocket _socketServer;
		unsigned short _serverPort;

	private:
		std::list<HSocket> _clientSockets;
		std::list<SocketMessage*> _UIMessageQueue;
		std::mutex   _UIMessageQueueMutex;

	};
}

NS_MT_END
  
#endif // MTSOCKET_H_ 
