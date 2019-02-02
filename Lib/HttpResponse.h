#pragma once
#include <string>
#include <map>

class HttpResponse
{
	int _code;
	std::string _httpVersion;
	std::string _message;
	std::map<std::string, std::string> _headers;

	char * _body;
public:

	HttpResponse(int);
	HttpResponse(int, std::string);

	std::string toString();
	void clearHeaders();
	void setHeaders(std::map<std::string, std::string>&);

	void setDefaultHttpVersion();

	HttpResponse();
	~HttpResponse();
};

