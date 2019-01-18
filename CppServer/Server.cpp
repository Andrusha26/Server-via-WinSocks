#include "pch.h"
#include "Server.h"

#include <ws2tcpip.h>
#include <iostream>
#include "WinSockHelper.h"
#include <thread>
#include <vector>
#include <exception>
#include <sstream>
#include <iomanip>

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
		closesocket(_listenSocket);
		WSACleanup();
	}
}

SOCKET &Server::acceptConnection()
{
	SOCKET clientSocket = accept(_listenSocket, NULL, NULL);

	if (clientSocket == INVALID_SOCKET) {
		std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(_listenSocket);
		WSACleanup();
	}

	return clientSocket;
}

void Server::acceptAndBroadcast()
{
	while (_running) {
		std::vector<std::thread*> threads;

		while (_currentConnections < MaxConnections && _running) {
			SOCKET clientSocket = acceptConnection();
			std::cout << "socket " << clientSocket << " was accepted." << std::endl;
			//broadcastMessages(clientSocket);
			std::thread *broadcastThread = new std::thread(&Server::broadcastMessages, this, clientSocket);
		}

		if (!_running) {
			std::cout << "server is forcibly closed." << std::endl;
		}

		for (std::vector<std::thread*>::iterator threadIterator = threads.begin();
			threadIterator != threads.end();
			threadIterator++) {
			(*threadIterator)->join();
		}
	}
}

void Server::broadcastMessages(SOCKET clientSocket)
{
	char * buffer = new char[DefaultBufferLength];

	int responseLength = receiveMessage(clientSocket, buffer);
    /*
	if (responseLength == 0) {
		closeConnection(clientSocket);
		return;
	}*/
	//TODO parse headers

	const char * responseString = "This is reponse";
	sendMessage(clientSocket, responseString);

	closeConnection(clientSocket);
}

int Server::receiveMessage(SOCKET & clientSocket, char * buffer)
{
	int recvResult = recv(clientSocket, buffer, DefaultBufferLength, 0);

	if (recvResult > 0) {
		std::cout << "Bytes received by " << clientSocket << " socket: " << recvResult << std::endl;
	}
	else if (recvResult == 0)
		return 0;
	else if (recvResult <= 0) {
		std::cout << "recv failed with error in " << clientSocket << " socket: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();

		_running = false;
		throw new std::exception("Receive is failed.");
	}

	return recvResult;
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
		closesocket(clientSocket);
		WSACleanup();

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
		closesocket(clientSocket);
		WSACleanup();
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

		winSockHelper.initializeSocket(Port);
		winSockHelper.bindListenSocket(_listenSocket);

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
}
