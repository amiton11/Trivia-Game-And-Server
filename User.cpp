#include "User.h"
#include "Helper.h"
#include "Defines.h"

User::User(string username, SOCKET sock)
{
	this->_sock = sock;
	this->_username = username;
	this->_currRoom = nullptr;
	this->_currGame = nullptr;

}

void User::send(string message)
{
	Helper::sendData(this->_sock, message);
}
string User::getUsername()
{
	return this->_username;
}
SOCKET User::getSocket()
{
	return this->_sock;
}
Room* User::getRoom()
{
	return this->_currRoom;
}
Game* User::getGame()
{
	this->_currRoom = nullptr;
	return this->_currGame;
}
void User::setGame(Game* game){
	this->_currGame = game;
}
void User::clearRoom(){
	this->_currRoom = nullptr;
}
bool User::CreateRoom(int roomId, string Roomname, int maxUsers, int questionNo, int questionTime)
{
	if (this->_currRoom == nullptr)
	{
		this->_currRoom = new Room(roomId, this, Roomname, maxUsers, questionNo, questionTime);
		Helper::sendData(this->_sock, string(P_CREATE_ROOM_SUCCESS));
		return true;
	}
	else
	{
		Helper::sendData(this->_sock, string(P_CREATE_ROOM_FAILED));
		return false;
	}
}

bool User::joinRoom(Room* room)
{
	if (this->_currGame != nullptr)
	{
		return false;
	}
	if (room->joinRoom(this))
	{
		this->_currRoom = room;
		return true;
	}
	else return false;
}

void User::leaveRoom()
{
	if (this->_currRoom != nullptr)
	{
		this->_currRoom->leaveRoom(this);
		this->_currRoom = nullptr;
	}
}

int User::closeRoom()
{
	if (this->_currRoom != nullptr || this->_currRoom->getUsers()[0] == this)
	{
		int i = this->_currRoom->closeRoom(this);
		if (i >= 0)
		{
			delete this->_currRoom;
			this->_currRoom = nullptr;
		}
		return i;
	}
	else return -1;
}

bool User::LeaveGame()
{
	if (this->getGame() == nullptr)
	{
		return false;
	}
	bool ret = this->getGame()->leaveGame(this);
	this->setGame(nullptr);
	return ret;
}
