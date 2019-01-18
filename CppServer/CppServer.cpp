// CppServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <string>
#include <iostream>

#include "Server.h"


int main()
{
	Server *server = new Server();
	server->start();
	system("pause");

	return 0;
}
