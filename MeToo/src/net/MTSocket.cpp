/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	MTSocket.cpp
// Author:		 Metoor
// Version: 	1.0
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#include "MTSocket.h"
#include "../base/MTUtils.h"

NS_MT_BEGIN

namespace net {

	//static variable 
	MTServerTCP* MTServerTCP::_server = nullptr;
	MTClientTCP* MTClientTCP::_client = nullptr;

	Protocol::Protocol(const Data& data)
		: _data(nullptr)
		, _length(0)
	{
		//copy protocol head
		strcpy(_head, PROTO_HEAD.c_str());
		_data = new Data(data);
	}

	Protocol::~Protocol()
	{
		if (_data)
		{
			MT_DELETE(_data);
		}
	}

	bool Protocol::check()
	{
		return PROTO_HEAD.compare(_head) == 0;
	}


	/************************************************************************/
	/*                           SocketMessage                              */
	/************************************************************************/
	SocketMessage::SocketMessage(MessageType type, const unsigned char* data, std::size_t dataLen, size_t extra)
		: _msgType(type)
		, _extra(extra)
		, _msgData(nullptr)
	{
		_msgType = type;
		_msgData = new metoo::Data;
		_msgData->copy(data, dataLen);
	}

	SocketMessage::SocketMessage(MessageType type, size_t extra)
		: _msgType(type)
		, _extra(extra)
		, _msgData(nullptr)
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
		sockAddr.sin_addr.s_addr = ip.empty() ? htonl(INADDR_ANY) : inet_addr(ip.c_str());
		return true;
	}

	bool MTSocket::ipv6Init(struct sockaddr_in6& sockAddr, HSocket & socketfd, const unsigned short port, const std::string & ip, int type)
	{

		return true;
	}

	/************************************************************************/
	/*                              MTServerTCP					   		  */
	/************************************************************************/
	MTServerTCP::MTServerTCP()
		: _socketServer(INVALID_ST)
		, _serverPort(0)
		, _isRunning(false)
		, _msgUpdateInterval(200)
		, _ip("")
		, onStart(nullptr)
		, onRecv(nullptr)
		, onNewConnection(nullptr)
		, onDisconnect(nullptr)
		, onError(nullptr)
	{
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

	bool MTServerTCP::startServer(unsigned short port)
	{
		if (!initServer(port))
		{
			return false;
		}

		_isRunning = true;

		messageLoop();

		return true;
	}

	void MTServerTCP::sendMessage(HSocket socket, const char* data, std::size_t count)
	{
		for (auto& sock : _clientSockets)
		{
			if (sock == socket)
			{
				int ret = send(socket, data, (int)count, 0);
				if (ret < 0)
				{
					std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
					auto errType = ErrorType::SEND_FAILED;
					auto msg = new SocketMessage(MessageType::INCORRECT, (unsigned char*)(&errType), sizeof(errType), socket);
					_UIMessageQueue.push_back(msg);
				}
				break;
			}
		}
	}

	void MTServerTCP::sendMessage(const char* data, std::size_t count)
	{
		for (auto& socket : _clientSockets)
		{
			int ret = send(socket, data, (int)count, 0);
			if (ret < 0)
			{
				std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
				auto errType = ErrorType::SEND_FAILED;
				auto msg = new SocketMessage(MessageType::INCORRECT, (unsigned char*)(&errType), sizeof(errType), socket);
				_UIMessageQueue.push_back(msg);
			}
		}
	}

	std::list<HSocket>::iterator MTServerTCP::findSocket(HSocket socket)
	{
		std::list<HSocket>::iterator iter = _clientSockets.begin();
		for (auto sct : _clientSockets)
		{
			if (sct == socket)
				return iter;
			++iter;
		}
		return _clientSockets.end();
	}

	void MTServerTCP::update(float dt)
	{
		if (_UIMessageQueue.size() == 0)
		{
			return;
		}

		_UIMessageQueueMutex.lock();

		if (_UIMessageQueue.size() == 0)
		{
			_UIMessageQueueMutex.unlock();
			return;
		}

		SocketMessage *msg = *(_UIMessageQueue.begin());
		_UIMessageQueue.pop_front();

		switch (msg->getMsgType())
		{
		case MessageType::START:
			if (onStart)
			{
				onStart(_ip.c_str(), _serverPort);
			}
			break;
		case MessageType::NEW_CONNECTION:
			if (onNewConnection)
			{
				auto data = msg->getMsgData();
				onNewConnection(*((HSocket*)data->getBytes()));
			}
			break;
		case MessageType::DISCONNECT:
			if (onDisconnect)
			{
				auto data = msg->getMsgData();
				HSocket clientSocket = *((HSocket*)data->getBytes());

				//delete disconnect client socket
				auto iter = findSocket(clientSocket);
				_clientSockets.erase(iter);
				onDisconnect(clientSocket);
			}
			break;
		case MessageType::RECEIVE:
			if (onRecv)
			{
				RecvData* recvData = (RecvData*)msg->getMsgData()->getBytes();
				onRecv(recvData->socketClient, recvData->data, recvData->dataLen);
			}
			break;
		case MessageType::INCORRECT:
		{
			auto type = msg->getMsgData()->getBytes();
			_onError((*(ErrorType*)(type)), msg->getMsgExtra());
		}
		break;
		default:
			break;
		}

		MT_DELETE(msg);
		_UIMessageQueueMutex.unlock();
	}

	void MTServerTCP::recvMessage(HSocket socket)
	{
		char buff[BUFFER_1K];
		int ret = 0;

		while (true)
		{
			ret = recv(socket, buff, sizeof(buff), 0);
			if (ret < 0)
			{
				std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
				auto errType = ErrorType::RECV_FAILED;
				auto msg = new SocketMessage(MessageType::INCORRECT, (unsigned char*)(&errType), sizeof(errType), socket);
				_UIMessageQueue.push_back(msg);
				break;
			}
			else
			{
				if (ret > 0 && onRecv != nullptr)
				{
					std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
					RecvData recvData;
					recvData.socketClient = socket;
					memcpy(recvData.data, buff, ret);
					recvData.dataLen = ret;
					SocketMessage * msg = new SocketMessage(MessageType::RECEIVE, (unsigned char*)&recvData, sizeof(RecvData));
					_UIMessageQueue.push_back(msg);
				}
			}
		}

		_mutex.lock();
		closeConnect(socket);
		if (onDisconnect != nullptr)
		{
			std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
			SocketMessage * msg = new SocketMessage(DISCONNECT, (unsigned char*)&socket, sizeof(HSocket));
			_UIMessageQueue.push_back(msg);
		}
		_mutex.unlock();
	}

	void MTServerTCP::clear()
	{
		if (_socketServer)
		{
			_mutex.lock();
			closeConnect(_socketServer);
			_mutex.unlock();
		}

		for (auto msg : _UIMessageQueue)
		{
			MT_DELETE(msg);
		}
		_UIMessageQueue.clear();
	}

	void MTServerTCP::messageLoop()
	{
		MTLOG("info: server message loop started!");
		acceptClient();
		while (_isRunning)
		{
			update(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

	bool MTServerTCP::initServer(unsigned short port)
	{
		do {
			int ret = 0;
			struct sockaddr_in serverAddr;
			ret = ipv4Init(serverAddr, _socketServer, port);
			if (!ret)
			{
				_onError(ErrorType::INIT_FAILED);
				break;
			}

			ret = bind(_socketServer, (const sockaddr*)&serverAddr, sizeof(serverAddr));
			if (ret < 0)
			{
				_onError(ErrorType::BIND_FAILED);
				break;
			}

			ret = listen(_socketServer, SOMAXCONN);
			if (ret < 0)
			{
				_onError(ErrorType::LISTEN_FAILED);
				break;
			}

			// start 
			char hostName[256];
			gethostname(hostName, sizeof(hostName));
			struct hostent* hostInfo = gethostbyname(hostName);
			_ip = inet_ntoa(*(struct in_addr *)*hostInfo->h_addr_list);

			//message loop can not start at now,so can not add lock.
			SocketMessage * msg = new SocketMessage(MessageType::START);
			_UIMessageQueue.push_back(msg);

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
		MTLOG("info: server started accept client connected!");
		int len = sizeof(sockaddr);
		struct sockaddr_in sockAddr;
		while (true)
		{
			HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, &len);
			if (IsError(clientSock))
			{
				_onError(ErrorType::ACCEPT_FAILED);
			}
			else
			{
				MTLOG("info: new client connected!");
				newClientConnected(clientSock);
			}
		}
	}

	void MTServerTCP::newClientConnected(HSocket socket)
	{
		_clientSockets.push_back(socket);
		std::thread th(&MTServerTCP::recvMessage, this, socket);
		th.detach();

		if (onNewConnection)
		{
			std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
			SocketMessage * msg = new SocketMessage(NEW_CONNECTION, (unsigned char*)&socket, sizeof(HSocket));
			_UIMessageQueue.push_back(msg);
		}
	}

	MTServerTCP::~MTServerTCP()
	{
		_isRunning = false;
		clear();
	}

	void MTServerTCP::_onError(ErrorType type, std::size_t extra)
	{
		if (onError)
		{
			onError(type, extra);
		}

		if (METOO_DEBUG)
		{

			std::string desc = "";

			switch (type)
			{
			case ErrorType::NONE:
				break;
			case ErrorType::INIT_FAILED:
				desc = "error: socket init failed!";
				break;
			case ErrorType::BIND_FAILED:
				desc = "error: socket bind failed!";
				break;
			case ErrorType::ACCEPT_FAILED:
				desc = "error: socket accept failed!";
				break;
			case ErrorType::LISTEN_FAILED:
				desc = "error: socket listen failed!";
				break;
			case ErrorType::SEND_FAILED:
				desc = StringUtils::format("error: socket send data to (%d) failed!", extra);
				break;
			case ErrorType::RECV_FAILED:
				desc = StringUtils::format("error: socket recv data from (%d) failed!", extra);
				break;
			case ErrorType::UNKNOW:
			default:
				desc = "error: unknow error!";
				break;
			}

			MTLOG(desc);
		}
	}

	/************************************************************************/
	/*                              MTClientTCP					   		  */
	/************************************************************************/
	MTClientTCP* MTClientTCP::getInstance()
	{
		if (_client == nullptr)
		{
			_client = new MTClientTCP();
		}
		return _client;
	}

	void MTClientTCP::destroyInstance()
	{
		MT_DELETE(_client);
	}

	bool MTClientTCP::connectServer(const char * serverIP, unsigned short port, bool msgLoopIsAsyn)
	{
		_serverIp = serverIP;
		_port = port;

		if (!initClient())
		{
			return false;
		}

		_isRunning = true;

		std::thread recvThread(&MTClientTCP::recvMessage, this);
		recvThread.detach();

		if (msgLoopIsAsyn)
		{
			std::thread messageLoopTh(&MTClientTCP::messageLoop, this);
			messageLoopTh.detach();
		}
		else
		{
			messageLoop();
		}

		return true;
	}

	void MTClientTCP::sendMessage(const char * data, std::size_t count)
	{
		if (_socektClient != INVALID_ST)
		{
			int ret = send(_socektClient, data, (int)count, 0);
			if (ret < 0)
			{
				_onError(ErrorType::SEND_FAILED);
			}
		}
	}

	MTClientTCP::MTClientTCP(void)
		: _serverIp("")
		, _port(0)
		, _isRunning(false)
		, onConnect(nullptr)
		, onRecv(nullptr)
		, onDisconnect(nullptr)
		, onError(nullptr)
		, _msgUpdateInterval(200)
		, _socektClient(INVALID_ST)
	{
	}

	void MTClientTCP::update(float dt)
	{
		if (_UIMessageQueue.size() == 0)
		{
			return;
		}

		_UIMessageQueueMutex.lock();

		if (_UIMessageQueue.size() == 0)
		{
			_UIMessageQueueMutex.unlock();
			return;
		}

		SocketMessage *msg = *(_UIMessageQueue.begin());
		_UIMessageQueue.pop_front();

		switch (msg->getMsgType())
		{
		case MessageType::START:
			if (onConnect)
			{
				onConnect();
			}
			break;
		case MessageType::DISCONNECT:
			_isRunning = false;
			if (onDisconnect)
			{
				onDisconnect();
			}
			break;
		case MessageType::RECEIVE:
			if (onRecv)
			{
				onRecv((const char*)msg->getMsgData()->getBytes(), msg->getMsgData()->getSize());
			}
			break;
		case MessageType::INCORRECT:
			break;
		default:
			break;
		}

		MT_DELETE(msg);
		_UIMessageQueueMutex.unlock();
	}

	MTClientTCP::~MTClientTCP(void)
	{
		_isRunning = false;
		clear();
	}

	void MTClientTCP::clear()
	{
		if (_socektClient != 0)
		{
			_mutex.lock();
			closeConnect(_socektClient);
			_mutex.unlock();
		}

		for (auto msg : _UIMessageQueue)
		{
			MT_DELETE(msg);
		}
		_UIMessageQueue.clear();

	}

	void MTClientTCP::messageLoop()
	{
		MTLOG("client message loop start!");
		while (_isRunning)
		{
			update(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(_msgUpdateInterval));
		}
		MTLOG("client message loop end!");
	}

	bool MTClientTCP::initClient()
	{
		do {
			int ret = 0;
			struct sockaddr_in serverAddr;
			ret = ipv4Init(serverAddr, _socektClient, _port, _serverIp);
			if (!ret)
			{
				_onError(ErrorType::INIT_FAILED);
				break;
			}

			ret = connect(_socektClient, (const sockaddr*)&serverAddr, sizeof(serverAddr));
			if (ret < 0)
			{
				_onError(ErrorType::CONNECT_FAILED);
				break;
			}

			//message loop can not start at now,so can not add lock.
			SocketMessage * msg = new SocketMessage(MessageType::START);
			_UIMessageQueue.push_back(msg);

			return true;
		} while (false);

		closeConnect(_socektClient);
		_socektClient = 0;
		return false;
	}

	void MTClientTCP::recvMessage()
	{
		char recvBuf[BUFFER_1K];
		int ret = 0;
		while (true)
		{
			ret = recv(_socektClient, recvBuf, sizeof(recvBuf), 0);
			if (ret < 0)
			{
				_onError(ErrorType::RECV_FAILED);
				break;
			}
			if (ret > 0 && onRecv != nullptr)
			{
				std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
				SocketMessage * msg = new SocketMessage(MessageType::RECEIVE, (unsigned char*)recvBuf, ret);
				_UIMessageQueue.push_back(msg);
			}
		}

		_mutex.lock();
		closeConnect(_socektClient);
		if (onDisconnect != nullptr)
		{
			std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
			SocketMessage * msg = new SocketMessage(MessageType::DISCONNECT);
			_UIMessageQueue.push_back(msg);
		}
		_socektClient = 0;
		_mutex.unlock();
	}

	void MTClientTCP::_onError(ErrorType type, std::size_t extra)
	{
		if (onError)
		{
			onError(type, extra);
		}

		if (METOO_DEBUG)
		{
			std::string desc = "";

			switch (type)
			{
			case ErrorType::NONE:
				break;
			case ErrorType::INIT_FAILED:
				desc = "error: socket init failed!";
				break;
			case ErrorType::CONNECT_FAILED:
				desc = StringUtils::format("error: socket connect server(%s:%d) failed!", _serverIp.c_str(), _port);
				break;
			case ErrorType::SEND_FAILED:
				desc = "error: socket send data failed!";
				break;
			case ErrorType::RECV_FAILED:
				desc = "error: socket recv data failed!";
				break;
			case ErrorType::UNKNOW:
			default:
				desc = "error: unknow error!";
				break;
			}

			MTLOG(desc);
		}
	}
}

NS_MT_END