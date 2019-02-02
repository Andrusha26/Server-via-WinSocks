#pragma once
#include <map>
#include <string>

class HttpRequest
{
	std::map<std::string, std::string> _headers;
	char * _body;
	int _bodyLength;
	int _requestLength;
	std::string _method;
	std::string _url;

	void parseHeaders(std::string &);
public:
	std::string  &getHeaderValueByKey(std::string&);
	char * getBody();
	int getBodyLength();
	int getRequestLength();
	std::string getMethod();
	std::string getUrl();

	HttpRequest(char *, int);
	HttpRequest();
	~HttpRequest();
};

