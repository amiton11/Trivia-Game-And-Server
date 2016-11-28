#pragma comment(lib, "Ws2_32.lib")

#include "TriviaServer.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
	try
	{
		TriviaServer server;
		server.server();
	}
	catch (std::string errMsg)
	{
		cout << errMsg << endl;
	}
	catch (...)
	{
		cout << "unknown error" << endl;
	}

	system("pause");
}