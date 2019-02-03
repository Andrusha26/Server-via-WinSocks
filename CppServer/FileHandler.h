#pragma once
#include "IHandler.h"
#include "HttpRequest.h"

class FileHandler : public IHandler
{
private:
	void receive(SOCKET,HttpRequest &);
	void sendResponse(SOCKET, HttpResponse &);
	HttpResponse * genereteOkResponse(std::string *);
public:
	void handle(SOCKET, HttpRequest &) override;
	HttpResponse * generateResponse() override;

	FileHandler();
	~FileHandler();
};

