#pragma once
#include "IHandler.h"

class HttpParser
{
public:
	IHandler *parseHttpRequest(char *);

	HttpParser();
	~HttpParser();
};

