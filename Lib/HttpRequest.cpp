#include "HttpRequest.h"
#include "StringHelper.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>

void HttpRequest::parseHeaders(std::string &headers)
{
	std::istringstream headersStream(headers);
	std::string eachLineToken;

	//parse method and url
	StringHelper::getLine(headersStream, eachLineToken);

	std::istringstream lineStream(eachLineToken);
	std::string wordBySpaceToken;

	std::getline(lineStream, wordBySpaceToken, ' ');
	_method = wordBySpaceToken;

	std::getline(lineStream, wordBySpaceToken, ' ');
	_url = wordBySpaceToken;

	//parse another key value pairs
	while(StringHelper::getLine(headersStream, eachLineToken)) {
		std::istringstream keyValuePairStream(eachLineToken);
		
		std::string key;
		std::getline(keyValuePairStream, key, ':');

		std::string value;
		std::getline(keyValuePairStream, value, ':');
	
		_headers.insert(std::pair<std::string, std::string>(key, value));
	}
}

std::string & HttpRequest::getHeaderValueByKey(std::string &key)
{

	std::map<std::string, std::string>::iterator mapValue = _headers.find(key);
	return mapValue->second;
}

char * HttpRequest::getBody()
{
	return _body;
}

size_t HttpRequest::getBodyLength()
{
	return _bodyLength;
}

size_t HttpRequest::getRequestLength()
{
	return _requestLength;
}

std::string HttpRequest::getMethod()
{
	return _method;
}

std::string HttpRequest::getUrl()
{
	return _url;
}

HttpRequest::HttpRequest(char * requestBuffer, int requestLength)
{
	std::string buffer(requestBuffer);
	std::string headers = buffer.substr(0, buffer.find("\r\n\r\n"));
	
	parseHeaders(headers);
	
	//4 - length of "\r\n\r\n"
	size_t headerLength = headers.length() + 4;
	_body = requestBuffer + headerLength;
	_bodyLength = requestLength - headerLength;
}

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}
