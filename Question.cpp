#include "Question.h"
#include <stdlib.h>
#include <iostream>
#include <ctime>

Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4)
{
	this->_id = id;
	this->_question = string(question);
	this->_answers[0] = "";
	this->_answers[1] = "";
	this->_answers[2] = "";
	this->_answers[3] = "";
	int j;
	for (int i = 0; i < 4; i++)
	{
		do
		{
			j = rand() % 4;
		} while (this->_answers[j] != "");
		if (i == 0)
		{
			this->_answers[j] = string(correctAnswer);
			this->_correctAnswerIndex = j;
		}
		else if (i == 1)
		{
			this->_answers[j] = string(answer2);
		}
		else if (i == 2)
		{
			this->_answers[j] = string(answer3);
		}
		else
		{
			this->_answers[j] = string(answer4);
		}
	}
}
int Question::getId()
{
	return this->_id;
}
int Question::getCorrectAnswerIndex()
{
	return this->_correctAnswerIndex;
}
string* Question::getAnswers()
{
	return this->_answers;
}
string Question::getQuestion()
{
	return this->_question;
}
