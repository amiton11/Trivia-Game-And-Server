#pragma once

#ifndef QUESTION_H
#define QUESTION_H


#include <string>

using namespace std;
class Question
{
private:
	int _id;
	string _question;
	string _answers[4];
	int _correctAnswerIndex;
public:
	Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4);
	int getId();
	int getCorrectAnswerIndex();
	string* getAnswers();
	string getQuestion();

};

#endif