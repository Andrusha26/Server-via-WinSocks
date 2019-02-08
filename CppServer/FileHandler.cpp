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

bool FileHandler::receive(SOCKET clientSocket, HttpRequest & request)
{
	if (request.getBodyLength() == 0)
		return false;

	std::ofstream fileStream;
	std::string fileName = std::to_string(clientSocket) + "receivedImage.png";
	fileStream.open(fileName, std::ios::binary);

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
		}
	}

	fileStream.close();
	delete fileBuffer;
	return true;
}

bool FileHandler::sendResponse(SOCKET clientSocket, HttpResponse & response)
{
	std::string responseString = response.toString();

	int sendResult = send(clientSocket, responseString.c_str(), (int)responseString.length(), 0);

	if (sendResult == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		WinSockHelper::cleanUp(clientSocket);

		return false;
	}

	return true;
}

HttpResponse *FileHandler::genereteOkResponse(const std::string *body)
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

HttpResponse *FileHandler::generateNotFoundResponse(const std::string *body)
{
	HttpResponse *response = new HttpResponse(404, "Not Found");

	std::map<std::string, std::string> headers;
	headers.insert(std::pair<std::string, std::string>("Version", "HTTP/1.1"));

	if (body->length() != 0)
		headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(body->length())));

	headers.insert(std::pair<std::string, std::string>("Connection", "Closed"));
	headers.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
	response->setHeaders(headers);

	response->setBody(body->c_str());

	return response;
}

void FileHandler::handle(SOCKET clientSocket, HttpRequest & request)
{
	bool receiveResult = receive(clientSocket, request);

	if (!receiveResult)
	{
		std::string *badResponseBody = new std::string("File is empty, or some error occured.");

		HttpResponse *badResponse = generateNotFoundResponse(badResponseBody);
		sendResponse(clientSocket, *badResponse);

		delete badResponseBody;
		return;
	}
	
	std::string *okResponseBody = new std::string("Image was downloaded");

	HttpResponse *response = genereteOkResponse(okResponseBody);
	sendResponse(clientSocket, *response);

	delete okResponseBody;
}

FileHandler::FileHandler()
{
}


FileHandler::~FileHandler()
{
}
