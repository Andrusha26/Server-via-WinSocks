#pragma once
#include <winsock2.h>
#include "HttpParser.h"

class Server
{
private:
	
	SOCKET _listenSocket;
	bool _running = false;

	int _currentConnections = 0;
	
	HttpParser * _httpParser;

	void addConnection();
	void releaseConnection();

	void startListening();
	SOCKET & acceptConnection();

	void acceptAndBroadcast();

	void broadcastMessages(SOCKET);
	int sendMessage(SOCKET &, const char* const&);

	void closeConnection(SOCKET &);
public:
	void start();

	Server();
	~Server();
};

