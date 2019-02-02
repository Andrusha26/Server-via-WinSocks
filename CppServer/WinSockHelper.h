#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class WinSockHelper
{
private:
	addrinfo *_listenAddrInfo = nullptr;

	void loadWinSock();

public:
	
	void initializeSocket(const char *);
	void bindListenSocket(SOCKET &);

	static void cleanUp(SOCKET);
	WinSockHelper();
	~WinSockHelper();
};

