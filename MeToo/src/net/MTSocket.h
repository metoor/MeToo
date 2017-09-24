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
		START,
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
		SEND_FAILED,
		RECV_FAILED,
		CONNECT_FAILED,
		UNKNOW
	};

	const unsigned int BUFFER_1K = 1024;
	const std::string PROTO_HEAD = "com.metoo.metoor";

	class Protocol {
	public:
		Protocol(const Data& data);
		virtual ~Protocol();

		inline std::size_t getSize() { return _length; };

	protected:
		//check the head is legal
		virtual bool check();

	private:
		char _head[17];
		std::size_t _length;
		Data* _data;
	};

	/************************************************************************/
	/*                           SocketMessage                              */
	/************************************************************************/
	class SocketMessage
	{
	public:
		//constructor
		SocketMessage(MessageType type, size_t extra = 0);
		SocketMessage(MessageType type, const unsigned char* data, std::size_t dataLen, size_t extra = 0);
		~SocketMessage();

		//getter
		inline const metoo::Data* getMsgData() { return _msgData; }
		inline const MessageType& getMsgType() { return _msgType; }
		inline const std::size_t getMsgExtra() { return _extra; }

	private:
		MessageType _msgType;
		metoo::Data* _msgData;
		std::size_t _extra;		//save extar parameter
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
		void sendMessage(HSocket socket, const char* data, std::size_t count);
		void sendMessage(const char* data, std::size_t count);
		void update(float dt);

		inline void setMsgUpdateInterval(unsigned int ms) { _msgUpdateInterval = ms; };
		inline unsigned int getMsgUpdateInterval(unsigned int ms) { return _msgUpdateInterval; };

		std::function<void(const char* ip, unsigned short port)> onStart;
		std::function<void(HSocket socket)> onNewConnection;
		std::function<void(HSocket socket, const char* data, std::size_t count)> onRecv;
		std::function<void(HSocket socket)> onDisconnect;
		std::function<void(ErrorType type, const std::string& des)> onError;

	protected:
		~MTServerTCP();

		//deal with error
		void _onError(ErrorType type, std::size_t extra = 0);

	private:
		MTServerTCP();
		bool initServer(unsigned short port);
		void acceptClient();
		void acceptFunc();
		void newClientConnected(HSocket socket);
		void recvMessage(HSocket socket);
		void clear();
		void mainLoop();

	private:
		struct RecvData
		{
			HSocket socketClient;
			int dataLen;
			char data[BUFFER_1K];
		};

	private:
		static MTServerTCP* _server;
		HSocket _socketServer;
		unsigned short _serverPort;
		std::string _ip;
		bool _isRunning;

		//default time is 200ms
		unsigned int _msgUpdateInterval;

	private:
		std::list<HSocket> _clientSockets;
		std::list<SocketMessage*> _UIMessageQueue;
		std::mutex   _UIMessageQueueMutex;

	};

	/************************************************************************/
	/*                              MTClientTCP					   		  */
	/************************************************************************/
	class MTClientTCP : public MTSocket
	{

	public:
		static MTClientTCP* getInstance();
		void destroyInstance();

		bool connectServer(const char* serverIP, unsigned short port);
		void sendMessage(const char* data, int count);

		std::function<void(const char* data, int count)> onRecv;
		std::function<void()> onDisconnect;

		void update(float dt);

	protected:
		~MTClientTCP(void);
		//deal with error
		void _onError(ErrorType type, std::size_t extra = 0);

	private:
		MTClientTCP(void);
		bool initClient();
		void recvMessage();
		void clear();

	private:
		std::string _serverIp;
		unsigned short _port;
		HSocket _socektClient;
		static MTClientTCP* _client;
		std::list<SocketMessage*> _UIMessageQueue;
		std::mutex   _UIMessageQueueMutex;
	};
}

NS_MT_END

#endif // MTSOCKET_H_ 
