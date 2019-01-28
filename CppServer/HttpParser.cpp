#include "pch.h"
#include "HttpParser.h"
#include "FileHandler.h"

#include <vector>
#include <iterator>
#include <sstream>

IHandler *HttpParser::parseHttpRequest(char * request)
{
	std::istringstream iss(request);
	std::vector<std::string> httpTokens((std::istream_iterator<std::string>(iss)),
		std::istream_iterator<std::string>());
	

	if (httpTokens[1] == "/file")
		return new FileHandler();
}

HttpParser::HttpParser()
{
}


HttpParser::~HttpParser()
{
}
