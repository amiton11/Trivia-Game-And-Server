#include "DataBase.h"
#include "Question.h"
#include "Defines.h"
#include <iostream>
#include <string>
#include <tuple>
#include "Helper.h"

using namespace std;


DataBase::DataBase()
{
	if (sqlite3_open(DATABASE_NAME, &this->_db))
	{
		sqlite3_close(this->_db);
		throw (string("Cant open db:") + string(sqlite3_errmsg(this->_db)));
	}
}

DataBase::~DataBase()
{
	sqlite3_close(this->_db);
}

bool DataBase::isUserExists(string user)
{
	bool exists = false;
	string sqlCommand = "SELECT * FROM t_users WHERE username=\'" + user + "\';";
	char *zErrMsg = 0;
	int rc = sqlite3_exec(_db, sqlCommand.c_str(), callbackCount, &exists, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
		return false;
	}
	return exists;
}

bool DataBase::addNewUser(string user, string pass, string email)
{
	string sqlCommand = "INSERT INTO t_users VALUES (\'" + user + "\',\'" + pass + "\',\'" + email + "\', 0);";
	char *zErrMsg = 0;
	int rc = sqlite3_exec(_db, sqlCommand.c_str(), callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}
	return true;
}

bool DataBase::isUserAndPassMatch(string user, string pass)
{
	return isUserExists(user + "\' AND password=\'" + pass);
}

std::vector<Question*> DataBase::initQuestions(int num)
{
	Question** questArr = new Question*[num];// were working on array because we not initalize the questions in the index order. dynamic becuase unknown lenths
	vector<Question*> quests;// were copying the array to here at the end.
	char *zErrMsg = NULL;
	int tableLenth = 0;//how many questions there is in the db
	string sqlCommand = "SELECT * FROM t_questions;";

	int rc = sqlite3_exec(_db, sqlCommand.c_str(), callbackTableLenth, &tableLenth, &zErrMsg);// checking the question table size;
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}
	vector<int> questionsIndex;
	if (tableLenth < num)
	{
		cout << "not enough questions" << endl;
		for (int i = 0; i < num; i++)
		{
			questionsIndex.push_back(rand() % tableLenth); // were doing a vetcor of: vector[question num] = question id where the id is random 
		}
	}
	else
	{
		int* numsChosen = new int[num - 1];
		for (int i = 0; i < num; i++)
		{
			int randNum = rand() % (tableLenth - i);
			for (int j = 0; j < i; j++)
			{
				if (numsChosen[j] <= randNum)
					randNum++;
				else
					break;
			}
			questionsIndex.push_back(randNum);
			if (i != num - 1)
				insertSorted(numsChosen, i, randNum);
		}
		delete[] numsChosen;
	}

	tuple<Question**, vector<int>*> params = make_tuple(questArr, &questionsIndex);
	rc = sqlite3_exec(_db, sqlCommand.c_str(), callbackQuestions, &params, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}

	quests = vector<Question*>(questArr, questArr + num);
	delete[] questArr;
	return quests;
}

void DataBase::insertSorted(int* arr, int len, int val)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (arr[i] > val)
		{
			for (int j = len; j > i; j--)
				arr[j] = arr[j - 1];
			arr[i] = val;
			return;
		}
	}
	if (i == len)
		arr[len] = val;
}

std::vector<std::string> DataBase::getBestScores()
{
	vector<string> bestScores;
	string sqlCmd = "SELECT username, total_score FROM t_users ORDER BY total_score DESC LIMIT 3;";

	char *zErrMsg = NULL;

	int rc = sqlite3_exec(_db, sqlCmd.c_str(), callbackBestScores, &bestScores, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}

	while (bestScores.size() < 3)
	{
		bestScores.push_back(Helper::getPaddedNumber(0, 2) + Helper::getPaddedNumber(0, 6));
	}

	return bestScores;
}

string DataBase::getPersonalStatus(std::string username)
{
	string sqlCmd = "SELECT is_correct, answer_time, game_id FROM t_players_answers WHERE username=\'" + username + "\';";
	int dataArr[3] = { 0, 0, 0 };
	vector<int> games;
	std::tuple<int*, vector<int>*> param = make_tuple(dataArr, &games);

	char *zErrMsg = NULL;

	int rc = sqlite3_exec(_db, sqlCmd.c_str(), callbackPersonalStatus, &param, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}
	double avgTime = 0;
	if (dataArr[0] + dataArr[1] != 0)
		avgTime = ((double)dataArr[2]) / ((double)(dataArr[0] + dataArr[1]));
	return (Helper::getPaddedNumber(games.size(), 4) + Helper::getPaddedNumber(dataArr[0], 6) + 
		Helper::getPaddedNumber(dataArr[1], 6) + Helper::getPaddedNumber((int)avgTime % 100, 2) + 
		Helper::getPaddedNumber((int)(avgTime * 100) % 100, 2));
}

int DataBase::insertNewGame()
{
	// insert to t_games the game with the current time
	string sqlCmd = "INSERT INTO t_games VALUES (null, 0, CURRENT_TIMESTAMP, null);";
	
	char *zErrMsg = 0;
	int rc = sqlite3_exec(_db, sqlCmd.c_str(), callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}
	// get the game's id
	sqlCmd = "SELECT game_id FROM t_games ORDER BY game_id DESC LIMIT 1;";
	int id = 0;
	rc = sqlite3_exec(_db, sqlCmd.c_str(), callbackId, &id, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}

	return id;
}

bool DataBase::updateGameStatus(int id)
{
	string sqlCmd = "UPDATE t_games SET status=1, end_time=CURRENT_TIMESTAMP WHERE game_id=" + to_string(id) + ";";

	char *zErrMsg = 0;
	int rc = sqlite3_exec(_db, sqlCmd.c_str(), callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}

	return true; // unnecessary but the UML chart said this function shoul return a bool
}

bool DataBase::addAnswerToPlayer(int gameId, std::string username, int questionId, std::string answer, bool isRight, int timeCount)
{
	string sqlCmd = "INSERT INTO t_players_answers VALUES (" + to_string(gameId) + ", \'" + username + "\', " + to_string(questionId)
		+ ", \'" + answer + "\', " + (isRight ? "1" : "0") + ", " + to_string(timeCount) + ");";

	char *zErrMsg = 0;
	int rc = sqlite3_exec(_db, sqlCmd.c_str(), callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		string temp = "SQL error: " + string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw (temp);
	}

	if (isRight)
	{
		sqlCmd = "UPDATE t_users SET total_score = total_score + 1 WHERE username=\'" + username + "\';";

		rc = sqlite3_exec(_db, sqlCmd.c_str(), callback, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			string temp = "SQL error: " + string(zErrMsg);
			sqlite3_free(zErrMsg);
			throw (temp);
		}
	}

	return true;
}

int DataBase::callbackCount(void *NotUsed, int argc, char **argv, char **azColName)
{
	bool* numAnswers = (bool*)NotUsed;
	*numAnswers = true;
	return 0;
}
int DataBase::callbackQuestions(void *NotUsed, int argc, char **argv, char **azColName)
{
	tuple<Question**, vector<int>*>* params = (tuple<Question**, vector<int>*>*) NotUsed;
	Question** questsPtr;
	vector<int>* questionIndex;
	tie(questsPtr, questionIndex) = *params;

	for (unsigned int i = 0;i < questionIndex->size();i++)
	{
		if (atoi(argv[0]) == (*questionIndex)[i] + 1)
		{
			Question* temp = new Question(stoi(string(argv[0])), string(argv[1]), string(argv[2]), string(argv[3]), string(argv[4]), string(argv[5]));
			questsPtr[i] = temp;
		}
	}return 0;
}
int DataBase::callbackBestScores(void *NotUsed, int argc, char **argv, char **azColName)
{
	vector<string>* bestScores = (vector<string>*) NotUsed;
	string username = string(argv[0]);
	int score = stoi(string(argv[1]));
	bestScores->push_back(Helper::getPaddedNumber(username.size(), 2) + username + Helper::getPaddedNumber(score, 6));
	return 0;
}
int DataBase::callbackPersonalStatus(void *NotUsed, int argc, char **argv, char **azColName)
{
	tuple<int*, vector<int>*>* param = (tuple<int*, vector<int>*>*) NotUsed;
	int* arr;
	int id = stoi(string(argv[2]));
	vector<int> *games;
	tie(arr, games) = *param;
	if (!strcmp(argv[0], "1"))
		arr[0]++;
	else if (!strcmp(argv[0], "0"))
		arr[1]++;
	arr[2] += stoi(string(argv[1]));
	
	if (find(games->begin(), games->end(), id) == games->end())
		games->push_back(id);

	return 0;
}
int DataBase::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	return 0;
}
int DataBase::callbackId(void *NotUsed, int argc, char **argv, char **azColName)
{
	int* id = (int*)NotUsed;
	string idStr = string(argv[0]);
	(*id) = stoi(idStr);
	return 0;
}


int DataBase::callbackTableLenth(void *NotUsed, int argc, char **argv, char **azColName)
{
	int* lenth = (int*)NotUsed;
	(*lenth)++;
	return 0;
}
