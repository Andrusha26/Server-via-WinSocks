#pragma once
#include "IHandler.h"
#include "HttpRequest.h"

class DefaultHandler : public IHandler
{
public:
	void handle(SOCKET, HttpRequest &) override;
	HttpResponse * generateResponse() override;
	DefaultHandler();
	~DefaultHandler();
};

