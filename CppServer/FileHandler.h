#pragma once
#include "IHandler.h"

class FileHandler : public IHandler
{
private:
	void receive(SOCKET, char *, int);
public:
	void handle(SOCKET, char *, int) override;
	FileHandler();
	~FileHandler();
};

