#include "RecievedMessage.h"

RecievedMessage::RecievedMessage(SOCKET s, int msgCode)
{
	this->_sock = s;
	this->_messageCode = msgCode;
	this->_user = nullptr;
	this->_values = std::vector<std::string>();
}

RecievedMessage::RecievedMessage(SOCKET s, int msgCode, std::vector<std::string> values)
{
	this->_sock = s;
	this->_messageCode = msgCode;
	this->_user = nullptr;
	for (unsigned int i = 0;i < values.size();i++)
	{
		this->_values.push_back(std::string(values[i]));
	}
}

SOCKET RecievedMessage::getSock()
{
	return this->_sock;
}

User * RecievedMessage::getUser()
{
	return this->_user;
}

int RecievedMessage::getMessageCode()
{
	return this->_messageCode;
}

std::vector<std::string>& RecievedMessage::getValues()
{
	return this->_values;
}

void RecievedMessage::setUser(User* user)
{
	this->_user = user;
}
