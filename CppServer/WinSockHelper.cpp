#include "pch.h"
#include "WinSockHelper.h"

#include <exception>
#include <iostream>

void WinSockHelper::loadWinSock()
{
	WSADATA wsaData;

	// Initialize Winsock
	int loadLibResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (loadLibResult != 0) {
		std::cout << "WSAStartup failed: \n" << loadLibResult << std::endl;
	}
}

void WinSockHelper::initializeSocket(const char * port)
{
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int addrInfoResult = getaddrinfo(nullptr, port, &hints, &_listenAddrInfo);

	if (addrInfoResult != 0) {
		std::cout << "getaddrinfo failed: \n" << addrInfoResult << std::endl;
		WSACleanup();

		throw new std::exception("Cannot initialize socket.");
	}
}

void WinSockHelper::bindListenSocket(SOCKET &listenSocket)
{
	listenSocket = socket(_listenAddrInfo->ai_family,
		_listenAddrInfo->ai_socktype,
		_listenAddrInfo->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		std::cout << "socket failed with error: \n" << WSAGetLastError() << std::endl;
		freeaddrinfo(_listenAddrInfo);
		WSACleanup();
	}

	// Setup the TCP listening socket
	int bindResult = bind(listenSocket,
		_listenAddrInfo->ai_addr,
		(int)_listenAddrInfo->ai_addrlen);

	if (bindResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: \n" << WSAGetLastError() << std::endl;
		freeaddrinfo(_listenAddrInfo);
		closesocket(listenSocket);
		WSACleanup();

		throw new std::exception("Cannot bind listen socket.");
	}

	//free because of unneed
	freeaddrinfo(_listenAddrInfo);
}

WinSockHelper::WinSockHelper()
{
	loadWinSock();
}


WinSockHelper::~WinSockHelper()
{
}
