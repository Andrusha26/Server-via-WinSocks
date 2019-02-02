#include "HttpResponse.h"
#include "Consts.h"

#include <sstream>


HttpResponse::HttpResponse(int code)
{
	setDefaultHttpVersion();
	_code = code;
}

HttpResponse::HttpResponse(int code, std::string message)
{
	setDefaultHttpVersion();
	_code = code;
	_message = message;
}

std::string HttpResponse::toString()
{
	std::stringstream response;
	response << _httpVersion << Consts::Space;
	response << _code << Consts::Space;
	response << _message << Consts::NewLine;

	for (std::map<std::string, std::string>::iterator hdrsItr = _headers.begin(); hdrsItr != _headers.end(); hdrsItr++) {
		response << (*hdrsItr).first << ": " << (*hdrsItr).second << Consts::NewLine;
	}

	response << Consts::NewLine << _body;

	return response.str();
}

void HttpResponse::clearHeaders()
{
	_headers.clear();
}

void HttpResponse::setHeaders(std::map<std::string, std::string> &headers)
{
	_headers = headers;
}

void HttpResponse::setDefaultHttpVersion()
{
	_httpVersion = "HTTP/1.1";
}

void HttpResponse::setBody(const char * body) {
	_body = body;
}

HttpResponse::HttpResponse()
{
	setDefaultHttpVersion();
}

HttpResponse::~HttpResponse()
{
}
