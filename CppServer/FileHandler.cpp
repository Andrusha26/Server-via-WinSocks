#include "pch.h"
#include "FileHandler.h"
#include "Consts.h"
#include "HttpResponse.h"
#include "WinSockHelper.h"

#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <map>

void FileHandler::receive(SOCKET clientSocket, HttpRequest & request)
{
	std::ofstream fileStream;
	fileStream.open("receivedImage.png", std::ios::binary);

	fileStream.write(request.getBody(), request.getBodyLength());

	bool isMoreAllowed = Consts::DefaultBufferLength != request.getRequestLength();
	
	char * fileBuffer = new char[Consts::DefaultBufferLength];

	while (isMoreAllowed) {
		int recvResult = recv(clientSocket, fileBuffer, Consts::DefaultBufferLength, 0);

		if (recvResult > 0) {
			std::cout << "Bytes received by " << clientSocket << " socket: " << recvResult << std::endl;
			fileStream.write(fileBuffer, recvResult);

			if (recvResult != Consts::DefaultBufferLength)
				isMoreAllowed = false;
		}
		else if (recvResult == 0)
			isMoreAllowed = false;
		else if (recvResult <= 0) {
			std::cout << "recv failed with error in " << clientSocket << " socket: " << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
			WSACleanup();

			isMoreAllowed = false;
			throw new std::exception("Receive is failed.");
		}
	}

	fileStream.close();
	delete fileBuffer;
}

void FileHandler::sendResponse(SOCKET clientSocket, HttpResponse & response)
{
	std::string responseString = response.toString();

	int sendResult = send(clientSocket, responseString.c_str(), responseString.length(), 0);

	if (sendResult == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		WinSockHelper::cleanUp(clientSocket);

		throw new std::exception("Cannot send message.");
	}
}

HttpResponse * FileHandler::genereteOkResponse(std::string *body)
{
	HttpResponse *response = new HttpResponse(200, "OK");

	std::map<std::string, std::string> headers;
	headers.insert(std::pair<std::string, std::string>("Version", "HTTP/1.1"));
	
	if(body->length() != 0) 
		headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(body->length())));

	headers.insert(std::pair<std::string, std::string>("Connection", "Closed"));
	headers.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
	response->setHeaders(headers);

	response->setBody(body->c_str());

	return response;
}

void FileHandler::handle(SOCKET clientSocket, HttpRequest & request)
{
	receive(clientSocket, request);
	HttpResponse * response = generateResponse();
	sendResponse(clientSocket, *response);

	delete response;
}

HttpResponse * FileHandler::generateResponse()
{
	std::string *responseMessage = new std::string("File is uploaded.");
	return genereteOkResponse(responseMessage);
}

FileHandler::FileHandler()
{
}


FileHandler::~FileHandler()
{
}
