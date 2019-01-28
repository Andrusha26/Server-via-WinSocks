#pragma once
#include <WinSock2.h>
class IHandler {
protected:
	const int DefaultBufferLength = 1024;
public:
	virtual void handle(SOCKET, char *, int) = 0;
};