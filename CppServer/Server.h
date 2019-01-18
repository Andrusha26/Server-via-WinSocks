#pragma once
#include <winsock2.h>

class Server
{
private:
	const char * Port = "27015";
	const int DefaultBufferLength = 1024;
	int MaxConnections = 1;

	SOCKET _listenSocket;
	bool _running = false;

	int _currentConnections = 0;
	
	void addConnection();
	void releaseConnection();

	void startListening();
	SOCKET & acceptConnection();

	void acceptAndBroadcast();

	void broadcastMessages(SOCKET);
	int receiveMessage(SOCKET &, char *);
	int sendMessage(SOCKET &, const char* const&);

	void closeConnection(SOCKET &);
public:
	void start();

	Server();
	~Server();
};

