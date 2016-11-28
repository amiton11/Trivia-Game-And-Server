#pragma once 
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include "Question.h"

class User;
class Room;
class DataBase;
using namespace std;

class Game
{
private:
	int _id;
    DataBase& _db;
	vector<Question*> _questions;
	vector<User*> _players;
	int _question_no;
	int _currQuestionIndex;
	map<string, int> _results;
	int _currentTurnAnswers;
public:
	Game(const vector<User*>& players, int questionsNo,DataBase& db);
	~Game();
	void sendQuestionToAllUsers();
	void handleFinishGame();
	void sendFirstQuestion();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user,int answerNo,int time);
	bool leaveGame(User* currUser);
};


#endif