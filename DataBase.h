#pragma once 
#ifndef DATABASE_H
#define DATABASE_H


#include <map>
#include <vector>
#include "sqlite3.h"

class Question;

class DataBase
{
private:
	sqlite3* _db;
	std::map<std::string, std::string> _temp;

	static int callbackCount(void*, int, char**, char**);
	static int callbackQuestions(void*, int, char**, char**);
	static int callbackBestScores(void*, int, char**, char**);
	static int callbackPersonalStatus(void*, int, char**, char**);
	static int callback(void*, int, char**, char**);
	static int callbackId(void*, int, char**, char**);
	static int callbackTableLenth(void*, int, char**, char**);
	static void insertSorted(int* arr, int len, int val);
public:
	DataBase();
	~DataBase();
	bool isUserExists(std::string);
	bool addNewUser(std::string, std::string, std::string);
	bool isUserAndPassMatch(std::string, std::string);
	std::vector<Question*> initQuestions(int);
	std::vector<std::string> getBestScores();
	std::string getPersonalStatus(std::string);
	int insertNewGame();
	bool updateGameStatus(int);
	bool addAnswerToPlayer(int, std::string, int, std::string, bool, int);

};

#endif