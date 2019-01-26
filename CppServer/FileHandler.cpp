#include "pch.h"
#include "FileHandler.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sstream>
#include <algorithm>

void FileHandler::receive(SOCKET clientSocket, char * requestBuffer, int firstBufferRequestLength)
{
	std::ofstream fileStream;
	fileStream.open("receivedImage.png", std::ios::binary);

	int headersLength = 0;

	for (char * charItr = requestBuffer; *(charItr + 3); charItr++) {
		headersLength++;

		if (*charItr == '\r' && *(charItr + 1) == '\n')
			if (*(charItr + 2) == '\r' && *(charItr + 3) == '\n')
				break;
	}

	headersLength += 3;
	char * firstRequestBodyBuffer = requestBuffer + headersLength;
	fileStream.write(firstRequestBodyBuffer, firstBufferRequestLength - headersLength);

	bool isMoreAllowed = DefaultBufferLength == firstBufferRequestLength;

	while (isMoreAllowed) {
		int recvResult = recv(clientSocket, requestBuffer, DefaultBufferLength, 0);

		if (recvResult > 0) {
			std::cout << "Bytes received by " << clientSocket << " socket: " << recvResult << std::endl;
			fileStream.write(requestBuffer, recvResult);

			if (recvResult != DefaultBufferLength)
				isMoreAllowed = false;
		}
		else if (recvResult == 0)
			isMoreAllowed = false;
		else if (recvResult <= 0) {
			std::cout << "recv failed with error in " << clientSocket << " socket: " << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
			WSACleanup();

			isMoreAllowed = false;
			throw new std::exception("Receive is failed.");
		}
	}

	fileStream.close();
}

void FileHandler::handle(SOCKET clientSocket, char * firstBufferRequest, int firstBufferResult)
{
	receive(clientSocket, firstBufferRequest, firstBufferResult);
}

FileHandler::FileHandler()
{
}


FileHandler::~FileHandler()
{
}
