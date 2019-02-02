#pragma once
#include <string>
#include <map>

class HttpResponse
{
	int _code;
	std::string _httpVersion;
	std::string _message;
	std::map<std::string, std::string> _headers;

	const char * _body;
public:

	HttpResponse(int);
	HttpResponse(int, std::string);

	std::string toString();
	void clearHeaders();
	void setHeaders(std::map<std::string, std::string>&);
	void setBody(const char *);

	void setDefaultHttpVersion();

	HttpResponse();
	~HttpResponse();
};

