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
#define INVALID_ST INVALID_SOCKET

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
#define INVALID_ST 0
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
		BIND_FAILED,
		ACCEPT_FAILED,
		LISTEN_FAILED,
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

		bool ipv4Init(struct sockaddr_in& sockAddr, HSocket& socketfd, const unsigned short port, const std::string& ip = "", int type = SOCK_STREAM);
		bool ipv6Init(struct sockaddr_in6& sockAddr, HSocket& socketfd, const unsigned short port, const std::string& ip = "", int type = SOCK_STREAM);

	protected:
		// ready for child
		std::mutex _mutex;

	private:
		bool _isInitSuccess;
	};

	/************************************************************************/
	/*                              MTServerTCP					   		  */
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

		std::function<void(const std::string& ip, const int port)> onStart;
		std::function<void(HSocket socket)> onNewConnection;
		std::function<void(HSocket socket, const char* data, int count)> onRecv;
		std::function<void(HSocket socket)> onDisconnect;
		std::function<void(ErrorType type, const std::string& des)> onError;

	protected:
		~MTServerTCP();

		//deal with error
		void _onError(ErrorType type, const std::string& des);

	private:
		MTServerTCP();
		bool initServer(unsigned short port);
		void acceptClient();
		void acceptFunc();
		void newClientConnected(HSocket socket);
		void recvMessage(HSocket socket);
		void clear();

	private:
		struct RecvData
		{
			HSocket socketClient;
			Data data;
		};

	private:
		static MTServerTCP* _server;
		HSocket _socketServer;
		unsigned short _serverPort;
		bool _isRunning;

	private:
		std::list<HSocket> _clientSockets;
		std::list<SocketMessage*> _UIMessageQueue;
		std::mutex   _UIMessageQueueMutex;

	};
}

NS_MT_END
  
#endif // MTSOCKET_H_ 
