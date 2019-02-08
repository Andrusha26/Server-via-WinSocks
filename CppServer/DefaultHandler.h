#pragma once
#include "IHandler.h"
#include "HttpResponse.h"

class DefaultHandler :public IHandler
{
	HttpResponse *generateNotFoundResponse(const std::string *);
	void sendResponse(SOCKET, HttpResponse &);
public:
	void handle(SOCKET, HttpRequest &) override;

	DefaultHandler();
	~DefaultHandler();
};

