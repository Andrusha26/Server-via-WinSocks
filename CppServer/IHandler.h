#pragma once
#include "HttpRequest.h"
#include "HttpResponse.h"

#include <WinSock2.h>
class IHandler {
public:
	virtual void handle(SOCKET, HttpRequest &) = 0;
	~IHandler();
};