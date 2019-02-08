#pragma once
#include "IHandler.h"
#include "HttpRequest.h"

class FileHandler : public IHandler
{
private:
	bool receive(SOCKET,HttpRequest &);
	bool sendResponse(SOCKET, HttpResponse &);

	HttpResponse *genereteOkResponse(const std::string *);
	HttpResponse *generateNotFoundResponse(const std::string *);
public:
	void handle(SOCKET, HttpRequest &) override;

	FileHandler();
	~FileHandler();
};

