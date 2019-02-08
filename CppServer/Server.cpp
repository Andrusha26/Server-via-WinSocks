#include "pch.h"
#include "Server.h"
#include "WinSockHelper.h"
#include "Consts.h"
#include "HttpRequest.h"

#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>

void Server::addConnection()
{
	_currentConnections++;
}

void Server::releaseConnection()
{
	if (_currentConnections > 0)
		_currentConnections--;
}

void Server::startListening()
{
	int listenResult = listen(_listenSocket, SOMAXCONN);

	if (listenResult == SOCKET_ERROR) {
		std::cout << "listen failed with error: \n" << WSAGetLastError() << std::endl;
		WinSockHelper::cleanUp(_listenSocket);
	}
}

 void Server::acceptConnection(SOCKET & socket)
{
	socket = accept(_listenSocket, NULL, NULL);

	if (socket == INVALID_SOCKET) {
		std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
		closeConnection(socket);
	}
}

void Server::acceptAndBroadcast()
{
	while (_running) {
		std::vector<std::thread*> threads;

		while (_currentConnections < Consts::MaxConnections && _running) {
			SOCKET clientSocket;
			acceptConnection(clientSocket);

			if (clientSocket == INVALID_SOCKET)
				continue;

			std::cout << "socket " << clientSocket << " was accepted." << std::endl;
			std::thread *broadcastThread = new std::thread(&Server::broadcastMessages, this, clientSocket);

			threads.push_back(broadcastThread);
			_currentConnections++;
		}

		if (!_running) {
			std::cout << "server is forcibly closed." << std::endl;
		}

		for (std::vector<std::thread*>::iterator threadIterator = threads.begin();
			threadIterator != threads.end();
			threadIterator++) {
			(*threadIterator)->join();
			delete *threadIterator;
		}
		_currentConnections = 0;
	}
}

void Server::broadcastMessages(SOCKET clientSocket)
{
	char * buffer = new char[Consts::DefaultBufferLength];
	bool connection = true;

	while (connection) {
		int recvResult = recv(clientSocket, buffer, Consts::DefaultBufferLength, 0);

		if (recvResult > 0) {
			HttpRequest request(buffer, recvResult);

			IHandler *handler = _httpParser->parseHttpRequest(request);
			handler->handle(clientSocket, request);

			delete handler;
			connection = false;
		}
		else if (recvResult <= 0) {
			std::cout << "recv failed with error " << recvResult << " in " << clientSocket << " socket: " << WSAGetLastError() << std::endl;
			
			connection = false;
		}
	}

	closeConnection(clientSocket);
	delete buffer;
}

void Server::closeConnection(SOCKET & clientSocket)
{
	int shutDownResult = shutdown(clientSocket, SD_SEND);
	if (shutDownResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: \n" << WSAGetLastError();
		WinSockHelper::cleanUp(clientSocket);
	}
	
	closesocket(clientSocket);

	std::cout << "socket " << clientSocket << " was closed." << std::endl;
}

void Server::start()
{
	startListening();
	acceptAndBroadcast();
}

Server::Server()
{
	try {
		WinSockHelper winSockHelper;

		winSockHelper.initializeSocket(Consts::Port);
		winSockHelper.bindListenSocket(_listenSocket);

		_httpParser = new HttpParser();

		_running = true;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
		_running = false;
	}
}


Server::~Server()
{
	delete _httpParser;
}
