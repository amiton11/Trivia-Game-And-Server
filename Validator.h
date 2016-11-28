#pragma once 

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
class Validator
{
public:
	static bool isPasswordValid(std::string pass);
	static bool isUsernameValid(std::string user);
};

#endif