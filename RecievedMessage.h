#pragma once 
#ifndef RECIEVEDMESSAGE_H
#define RECIEVEDMESSAGE_H

#include <WinSock2.h>
#include <vector>

class User;

class RecievedMessage
{
private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	std::vector<std::string> _values;

public:
	RecievedMessage(SOCKET, int);
	RecievedMessage(SOCKET, int, std::vector<std::string>);

	SOCKET getSock();
	User* getUser();
	int getMessageCode();
	std::vector<std::string>& getValues();

	void setUser(User*);

};

#endif