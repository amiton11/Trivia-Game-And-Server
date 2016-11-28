#include "Validator.h"

bool Validator::isPasswordValid(std::string pass)
{
	bool containsD = false, containsL = false, containsU = false;
	if (pass.length() < 4)
	{
		return false;
	}
	for (unsigned int i = 0;i < pass.length(); i++)
	{
		if (pass[i] == ' ')
		{
			return false;
		}
		if (pass[i] >= '0' && pass[i] <= '9')
		{
			containsD = true;
		}
		if (pass[i] >= 'a' && pass[i] <= 'z')
		{
			containsL = true;
		}
		if (pass[i] >= 'A' && pass[i] <= 'Z')
		{
			containsU = true;
		}
	}
	return (containsD && containsL && containsU);
}

bool Validator::isUsernameValid(std::string user)
{
	if (user.length() == 0)
	{
		return false;
	}
	for (unsigned int i = 0;i < user.length();i++)
	{
		if(user[i] == ' ')
		{
			return false;
		}
	}
	return (user[0] >= 'a' && user[0] <= 'z') || (user[0] >= 'A' && user[0] <= 'Z');
}