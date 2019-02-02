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

SOCKET &Server::acceptConnection()
{
	SOCKET clientSocket = accept(_listenSocket, NULL, NULL);

	if (clientSocket == INVALID_SOCKET) {
		std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
		WinSockHelper::cleanUp(_listenSocket);
	}

	return clientSocket;
}

void Server::acceptAndBroadcast()
{
	while (_running) {
		std::vector<std::thread*> threads;

		//TODO add release logic
		while (_currentConnections < Consts::MaxConnections && _running) {
			SOCKET clientSocket = acceptConnection();
			std::cout << "socket " << clientSocket << " was accepted." << std::endl;
			std::thread *broadcastThread = new std::thread(&Server::broadcastMessages, this, clientSocket);

			threads.push_back(broadcastThread);
		}

		if (!_running) {
			std::cout << "server is forcibly closed." << std::endl;
		}

		for (std::vector<std::thread*>::iterator threadIterator = threads.begin();
			threadIterator != threads.end();
			threadIterator++) {
			(*threadIterator)->join();
			//TODO delete thread?
		}
	}
}

void Server::broadcastMessages(SOCKET clientSocket)
{
	char * buffer = new char[Consts::DefaultBufferLength];
	while (_running) {
		int recvResult = recv(clientSocket, buffer, Consts::DefaultBufferLength, 0);

		if (recvResult > 0) {
			HttpRequest request(buffer, recvResult);

			IHandler *handler = _httpParser->parseHttpRequest(request);
			handler->handle(clientSocket, request);
			//delete handler;
		}
		else if (recvResult <= 0) {
			std::cout << "recv failed with error in " << clientSocket << " socket: " << WSAGetLastError() << std::endl;
			WinSockHelper::cleanUp(clientSocket);

			_running = false;
		}
	}

	closeConnection(clientSocket);
	delete buffer;
}

int Server::sendMessage(SOCKET & clientSocket, const char * const & body = nullptr)
{
	std::stringstream response;
	response <<
		"HTTP/1.1 200 OK" << std::endl <<
		"Version: HTTP/1.1" << std::endl;

		if (body != nullptr)
			response << "Content-Length: " << strlen(body) << std::endl;
		 
	response <<
		"Content-Type: text/html" << std::endl <<
		"Connection: Closed" << std::endl;

	response << std::endl << body;

	////////////////////////////////////////////////
	std::string TESTREMOVE = response.str();
	int length = response.str().length();
	////////////////////////////////////////////////

	// Echo the buffer back to the sender
	int sendResult = send(clientSocket, response.str().c_str(), response.str().length(), 0);
	
	if (sendResult == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		WinSockHelper::cleanUp(clientSocket);

		_running = false;
		throw new std::exception("Cannot send message.");
	}

	std::cout << "Bytes sent to " << clientSocket << " socket: " << sendResult << std::endl;
	return sendResult;
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
