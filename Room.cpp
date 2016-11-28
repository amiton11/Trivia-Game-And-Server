#include "Room.h"
#include "Helper.h"
#include "User.h"
#include "Defines.h"

void Room::sendMessage(User* excluded, string message)
{
	for (unsigned int i = 0;i < this->_users.size();i++)
	{
		if (this->_users[i] != excluded)
		{
			try
			{
				this->_users[i]->send(message);
			}
			catch (...)
			{

			}
		}
	}
}
void Room::sendMessage(string message)
{
	this->sendMessage(nullptr, message);
}
Room::Room(int id, User* admin, string name, int maxUsers, int questionNo, int questionTime)
{
	this->_id = id;
	this->_admin = admin;
	this->_name = name;
	this->_maxUsers = maxUsers;
	this->_questionNo = questionNo;
	this->_questionTime = questionTime;
	this->_users = vector<User*>();
	this->_users.push_back(admin);
}

vector<User*> Room::getUsers()
{
	return this->_users;
}

int Room::getQuestionsNo()
{
	return this->_questionNo;
}

int Room::getId()
{
	return this->_id;
}

string Room::getName()
{
	return this->_name;
}

string Room::getUserListMessage()
{
	string retString = P_SEND_USERS_IN_ROOM;
	retString += Helper::getPaddedNumber(this->_users.size(),1);
	for (unsigned int i = 0;i < this->_users.size();i++)
	{
		retString += Helper::getPaddedNumber(this->_users[i]->getUsername().length(), 2);
		retString += this->_users[i]->getUsername();
	}
	return retString;
}

bool Room::joinRoom(User* user)
{
	if (this->_users.size() >= this->_maxUsers)
	{
		Helper::sendData(user->getSocket(), string(P_JOIN_ROOM_ROOM_FULL));
		return false;
	}
	Helper::sendData(user->getSocket(), string(P_JOIN_ROOM_SUCCESS) + Helper::getPaddedNumber(this->_questionNo, 2) + Helper::getPaddedNumber(this->_questionTime, 2));
	this->_users.push_back(user);
	this->sendMessage(this->getUserListMessage());
	return true;
}

void Room::leaveRoom(User* user)
{
	for (unsigned int i = 0;i < this->_users.size();i++)
	{
		if (this->_users[i] == user)
		{
			_users.erase(_users.begin() + i);
			Helper::sendData(user->getSocket(), P_LEAVE_ROOM_SUCCESS);
			this->sendMessage(user, this->getUserListMessage());
		}
	}
}

int Room::closeRoom(User* user)
{
	if (user != this->_admin)
	{
		return -1;
	}
	this->sendMessage(P_CLOSE_ROOM);
	for (unsigned int i = 0;i < this->_users.size();i++)
	{
		if (this->_users[i] != this->_admin)
		{
			this->_users[i]->clearRoom();
		}
	}
	return this->_id;
}
