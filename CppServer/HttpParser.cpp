#include "pch.h"
#include "HttpParser.h"
#include "FileHandler.h"
#include "HttpRequest.h"
#include "DefaultHandler.h"

#include <vector>
#include <iterator>
#include <sstream>

IHandler *HttpParser::parseHttpRequest(HttpRequest &request)
{
	if (request.getUrl() == "/file")
		return new FileHandler();

	return new DefaultHandler();
}

HttpParser::HttpParser()
{
}


HttpParser::~HttpParser()
{
}
