#pragma once 
#ifndef TRIVIASERVER_H
#define TRIVIASERVER_H

#include "Defines.h"
#include <WinSock2.h>
#include <map>
#include <queue>
#include "DataBase.h"
#include <mutex>
#include <condition_variable>

class User;
class Room;
class RecievedMessage;

class TriviaServer
{
private:
	SOCKET _socket;
	DataBase _db;
	std::map<SOCKET, User*> _connectedUsers;
	std::map<int, Room*> _roomsList;
	std::mutex _mtxRecievedMessages;
	std::condition_variable _cv;
	WSADATA _wsaData;

	std::queue<RecievedMessage*> _queRcvMessages;
	int _roomIdSequence;

	void bindAndListen();
	void accept();
	void ClientHandler(SOCKET);
	void safeDeleteUser(RecievedMessage*);

	User* handleSignIn(RecievedMessage*);
	bool handleSignUp(RecievedMessage*);
	void handleSignOut(RecievedMessage*);

	void handleLeaveGame(RecievedMessage*);
	void handleStartGame(RecievedMessage*);
	void handlePlayerAnswer(RecievedMessage*);

	bool handleCreateRoom(RecievedMessage*);
	bool handleCloseRoom(RecievedMessage*);
	bool handleJoinRoom(RecievedMessage*);
	bool handleLeaveRoom(RecievedMessage*);
	void handleGetUserInRoom(RecievedMessage*);
	void handleGetRooms(RecievedMessage*);

	void handleGetBestScores(RecievedMessage*);
	void handleGetPersonalStatus(RecievedMessage*);

	void handleRecievedMessage();
	void addRecievedMessage(RecievedMessage*);
	RecievedMessage* buildRecieveMessage(SOCKET, int);


	User* getUserByName(std::string);
	User* getUserBySocket(SOCKET);
	Room* getRoomById(int);


public:
	TriviaServer();
	~TriviaServer();
	void server();
};

void pushFromSock(SOCKET sock, std::vector<std::string>* vec,int length=2);

#endif