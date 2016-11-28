#include "Game.h"
#include "User.h"
#include "Helper.h"
#include "DataBase.h"
#include "Defines.h"


Game::Game(const vector<User*>& players, int questionsNo, DataBase& db) : _db(db)
{
	this->_players = move(players);
	this->_id = this->_db.insertNewGame();
	this->_question_no = questionsNo;
	this->_questions = this->_db.initQuestions(questionsNo);
	this->_results = map<string, int>();
	for (unsigned int i = 0;i < this->_players.size();i++)
	{
		this->_players[i]->setGame(this);
		this->_results.insert(pair<string,int>(this->_players[i]->getUsername(), 0));
	}
	this->_currentTurnAnswers = 0;
	this->_currQuestionIndex = 0;
}

Game::~Game()
{
	for (unsigned int i = 0;i < this->_questions.size();i++)
	{
		delete this->_questions[i];
	}
	this->_players.clear();
	this->_questions.clear();
}

void Game::sendQuestionToAllUsers()
{
	Question* q = this->_questions[this->_currQuestionIndex];
	string msg = P_SEND_QUESTION;
	msg += Helper::getPaddedNumber(q->getQuestion().length(), 3);
	msg += q->getQuestion();
	for (int i = 0;i < 4;i++)
	{
		msg += Helper::getPaddedNumber(q->getAnswers()[i].length(), 3);
		msg += q->getAnswers()[i];
	}
	this->_currentTurnAnswers = 0;
	for (unsigned int i = 0;i < this->_players.size();i++)
	{
		try
		{
			this->_players[i]->send(msg);
		}
		catch(...)
		{

		}
	}
}

void Game::handleFinishGame()// calls db, have catch
{
	this->_db.updateGameStatus(this->_id);
	string temp;
	temp = string(P_END_GAME);
	temp += Helper::getPaddedNumber(this->_players.size(), 1);
	for (unsigned int i = 0; i < this->_players.size(); i++)
	{
		temp += Helper::getPaddedNumber(this->_players[i]->getUsername().length(), 2);
		temp += this->_players[i]->getUsername();
		temp += Helper::getPaddedNumber(this->_results[this->_players[i]->getUsername()], 2);
	}
	for (unsigned int i = 0; i < this->_players.size(); i++)
	{
		try
		{
			this->_players[i]->send(temp);
		}
		catch (...)
		{

		}
		this->_players[i]->setGame(nullptr);
	}
}

void Game::sendFirstQuestion()
{
	this->sendQuestionToAllUsers();
}

bool Game::handleNextTurn()
{
	if (this->_players.size() == 0)
	{
		this->handleFinishGame();
		return false;
	}
	if (this->_currentTurnAnswers == this->_players.size())
	{
		if (++this->_currQuestionIndex == this->_question_no)
		{
			this->handleFinishGame();
			return false;
		}
		this->sendQuestionToAllUsers();
	}
	return true;
}

bool Game::handleAnswerFromUser(User * user, int answerNo, int time)
{
	Question* q = this->_questions[this->_currQuestionIndex];
	this->_currentTurnAnswers++;
	bool isCorrect = answerNo == q->getCorrectAnswerIndex() + 1;
	string answer = answerNo < 5 ?  q->getAnswers()[q->getCorrectAnswerIndex()] : "";
	if (isCorrect)
	{
		this->_results[user->getUsername()]++;
		user->send(P_ANSWER_RIGHT);
	}
	else
	{
		user->send(P_ANSWER_WRONG);
	}
	this->_db.addAnswerToPlayer(this->_id, user->getUsername(), q->getId(), answer , isCorrect, time);
	return this->handleNextTurn();
}

bool Game::leaveGame(User* currUser)
{
	for (unsigned int i = 0;i < this->_players.size();i++)
	{
		if (this->_players[i] == currUser)
		{
			this->_players.erase(this->_players.begin() + i);
			return this->handleNextTurn();
		}
	}
	return false;
}
