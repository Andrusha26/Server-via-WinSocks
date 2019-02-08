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
	void acceptConnection(SOCKET&);

	void acceptAndBroadcast();

	void broadcastMessages(SOCKET);
	void closeConnection(SOCKET &);
public:
	void start();

	Server();
	~Server();
};

