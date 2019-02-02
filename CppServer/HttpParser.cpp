#include "pch.h"
#include "HttpParser.h"
#include "FileHandler.h"
#include "HttpRequest.h"

#include <vector>
#include <iterator>
#include <sstream>

IHandler *HttpParser::parseHttpRequest(HttpRequest &request)
{
	if (request.getUrl() == "/file")
		return new FileHandler();
}

HttpParser::HttpParser()
{
}


HttpParser::~HttpParser()
{
}
