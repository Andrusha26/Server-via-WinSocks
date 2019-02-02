#pragma once
#include "IHandler.h"
#include "HttpRequest.h"

class HttpParser
{
public:
	IHandler *parseHttpRequest(HttpRequest &);

	HttpParser();
	~HttpParser();
};

