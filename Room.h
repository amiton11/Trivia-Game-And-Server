#pragma once 

#ifndef ROOM_H
#define ROOM_h

#include <vector>
#include <string>
using namespace std;

class User;

class Room
{
private:
	std::vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionNo;
	string _name;
	int _id;


	void sendMessage(User* excluded, string message);
	void sendMessage(string message);
public:
	Room(int id, User* admin, string name, int maxUsers, int questionNo, int questionTime);
	vector<User*> getUsers();
	int getQuestionsNo();
	int getId();
	string getName();
	string getUserListMessage();
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);
};

#endif