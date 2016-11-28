#pragma once 
#ifndef USER_H
#define USER_H





#include <string>
#include "Room.h"
#include "Game.h"
#include <WinSock2.h>
using namespace std;


class User
{
private:
	string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;
public:
	User(string username, SOCKET sock);
	void send(string message);
	string getUsername();
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	void setGame(Game* game);
	void clearRoom();
	bool CreateRoom(int roomId, string Roomname, int maxUsers, int questionNo, int questionTime);
	bool joinRoom(Room* room);
	void leaveRoom();
	int closeRoom();
    bool LeaveGame();
};

#endif