#include "pch.h"
#include "DefaultHandler.h"
#include "WinSockHelper.h"

#include <iostream>

HttpResponse *DefaultHandler::generateNotFoundResponse(const std::string *body)
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

void DefaultHandler::sendResponse(SOCKET clientSocket, HttpResponse &response)
{
	std::string responseString = response.toString();

	int sendResult = send(clientSocket, responseString.c_str(), (int)responseString.length(), 0);

	if (sendResult == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		WinSockHelper::cleanUp(clientSocket);

		throw new std::exception("Cannot send message.");
	}
}

void DefaultHandler::handle(SOCKET clientSocket, HttpRequest & request)
{
	std::string *responseBody = new std::string(request.getUrl() + " is not found, please try again");
	HttpResponse *response = generateNotFoundResponse(responseBody);
	sendResponse(clientSocket, *response);

	delete response;
}

DefaultHandler::DefaultHandler()
{
}


DefaultHandler::~DefaultHandler()
{
}
