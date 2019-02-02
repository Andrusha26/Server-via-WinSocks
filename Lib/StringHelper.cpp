#include "StringHelper.h"
#include <string>

std::istream & StringHelper::getLine(std::istream & os, std::string & line)
{
	std::string myline;
	if (std::getline(os, myline)) {
		if (myline.size() && myline[myline.size() - 1] == '\r') {
			line = myline.substr(0, myline.size() - 1);
		}
		else {
			line = myline;
		}
	}
	return os;
}
