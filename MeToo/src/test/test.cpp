/*************************************************
// Copyright (C), 2016-2017, CS&S. Co., Ltd.
// File name: 	test.cpp
// Author:		 Metoor
// Version: 	1.0 
// Date: 		2017/09/21
// Contact: 	caiyoufen@gmail.com
// Description: 	create by vs2015pro
*************************************************/

#include "../base/metoo.h"

USING_NS_MT;

using namespace std;

void print(const string& funcName, bool isSucceed)
{
	log("%s ---> %s", funcName.c_str(), isSucceed ? "test ok" : "test failed");
}

// utils test function
void test_utils() 
{
	log("----------utils test start-----------");
	
	//test format
	auto ss = StringUtils::format("%sddwed%d", "ttttttt", 999);
	print("StringUtils::format", ss.compare("tttttttddwed999") == 0);

	//test split
	auto sv = StringUtils::split("1*2*3", '*');
	int count = 0;
	int i = 0;
	for each(auto var in sv)
	{
		++i;
		if (var.compare(StringUtils::format("%d", i)) == 0)
			++count;
	}
	print("StringUtils::split", count == sv.size());

	//test trim
	auto st = StringUtils::trim(" i am ok! ");
	print("StringUtils::trim", st.compare("i am ok!") == 0);

	log("----------utils test end -----------");
}

#include <iostream>
using namespace std;

int main()
{
	log(metoo::metooVersion());

	//test_utils();


	auto server = metoo::net::MTServerTCP::getInstance();
	server->onStart = [] (const string& ip, unsigned short port){
		log("server started on ip:" + ip);
	};

	server->onDisconnect = [](SOCKET socket) {
		cout << socket << " disconnect" << endl;
	};

	server->onNewConnection = [&server](SOCKET socket) {
		cout << socket << " connected" << endl;
		server->sendMessage(socket, "Wellcome connect!", 18);
	};

	server->onRecv = [&server](SOCKET socket, const char* data, int lenth) {
		cout << " recv £º" << socket << " data£º" << data << endl;
	};

	server->startServer(6666);

	return 0;
}