#include "TriviaServer.h"
#include "Helper.h"
#include "User.h"
#include "Room.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "RecievedMessage.h"
#include "Validator.h"

//SOCKET _socket;
//DataBase _db();
//map<SOCKET, User*> _connectedUsers;
//map<int, Room*> _roomsList;
//mutex _mtxRecievedMessages;
//queue<RecievedMessage*> _queRcvMessages;
//int _roomIdSequence;

TriviaServer::TriviaServer() : _connectedUsers(), _roomsList(), _mtxRecievedMessages(), _queRcvMessages()
{
	int result = WSAStartup(MAKEWORD(2, 2), &_wsaData);
	if (result != 0) 
	{
		throw string("WSAStartup failed: %d\n", result);
	}
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw string("couldn't create socket");
	_roomIdSequence = 0;
}
TriviaServer::~TriviaServer()
{
	for (map<SOCKET, User*>::iterator irt = _connectedUsers.begin(); irt != _connectedUsers.end(); irt++)
	{
		delete irt->second;
	}
	for (map<int, Room*>::iterator irt = _roomsList.begin(); irt != _roomsList.end(); irt++)
	{
		delete irt->second;
	}
	while (!_queRcvMessages.empty())
	{
		delete _queRcvMessages.front();
		_queRcvMessages.pop();
	}
}
void TriviaServer::server()
{
	bindAndListen();	
	thread* handlingThread = new thread(&TriviaServer::handleRecievedMessage, this);
	try
	{
		while (true)
		{
			accept();
		}
	}
	catch (...)
	{
		delete handlingThread;
	}
}

void TriviaServer::bindAndListen()
{
	struct sockaddr_in svrAdd;

	memset(&svrAdd, 0, sizeof(svrAdd));

	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(SERVER_PORT);

	if (::bind(_socket, (const sockaddr*)&svrAdd, sizeof(svrAdd)) == SOCKET_ERROR)
		throw string("can't bind socket");

	listen(_socket, MAX_USERS);

	cout << "Listening..." << endl;
}

void TriviaServer::accept()
{
	struct sockaddr_in clntAdd;
	memset(&clntAdd, 0, sizeof(clntAdd));

	int len = sizeof(clntAdd);
	SOCKET clntSock = ::accept(_socket, (sockaddr*)&clntAdd, &len);

	if (clntSock == INVALID_SOCKET)
	{
		cerr << "couldn't accept client" << endl;
		return;
	}

	thread handler(&TriviaServer::ClientHandler, this, clntSock);
	handler.detach();
}

void TriviaServer::ClientHandler(SOCKET sock)
{
	try
	{
		int messageType = Helper::getMessageTypeCode(sock);
		while (messageType != 0 && messageType != REQ_EXIT_APP)
		{
			addRecievedMessage(buildRecieveMessage(sock, messageType));

			messageType = Helper::getMessageTypeCode(sock);
		}
		addRecievedMessage(buildRecieveMessage(sock, REQ_EXIT_APP));
	}	
	catch (...)
	{
		RecievedMessage* tempMsg = new RecievedMessage(sock, REQ_EXIT_APP);
		tempMsg->setUser(getUserBySocket(sock));
		safeDeleteUser(tempMsg);
	}
}

void TriviaServer::safeDeleteUser(RecievedMessage* msg)
{
	try
	{
		handleSignOut(msg);

		closesocket(msg->getSock());
	}
	catch (...){}
}

User* TriviaServer::handleSignIn(RecievedMessage* msg)
{
	string username = msg->getValues()[0];//for prettier code
	string password = msg->getValues()[1];
	if (this->_db.isUserAndPassMatch(username, password))
	{
		if (this->getUserByName(username) == nullptr) //checks if this user is already connected
		{
			Helper::sendData(msg->getSock(), P_SIGN_IN_SUCCESS);
			User* newuser = new User(username, msg->getSock());
			this->_connectedUsers.insert(pair<SOCKET, User*>(msg->getSock(), newuser));
			return newuser;
		}
		else Helper::sendData(msg->getSock(), P_SIGN_IN_ALREADY_CONNECTED);
	}
	else Helper::sendData(msg->getSock(), P_SIGN_IN_FAILED);
	return nullptr;
}
bool TriviaServer::handleSignUp(RecievedMessage* msg)
{
	string username = msg->getValues()[0];
	string password = msg->getValues()[1];
	string email = msg->getValues()[2];
	if (!Validator::isPasswordValid(password))
	{
		Helper::sendData(msg->getSock(), P_SIGN_UP_PASS_ILLEGAL);
		return false;
	}
	if (!Validator::isUsernameValid(username))
	{
		Helper::sendData(msg->getSock(), P_SIGN_UP_USER_ILLEGAL);
		return false;
	}
	if (this->_db.isUserExists(username))
	{
		Helper::sendData(msg->getSock(), P_SIGN_UP_USER_EXISTS);
		return false;
	}
	if (this->_db.addNewUser(username, password, email))
	{
		Helper::sendData(msg->getSock(), P_SIGN_UP_SUCCESS);
		return true;
	}
	else
	{
		Helper::sendData(msg->getSock(), P_SIGN_UP_OTHER);
		return false;
	}
}
void TriviaServer::handleSignOut(RecievedMessage* msg)
{
	if (msg->getUser() == nullptr)
		return;
	_connectedUsers.erase(msg->getSock());
	if (!handleCloseRoom(msg))
		handleLeaveRoom(msg);
	handleLeaveGame(msg);
}

void TriviaServer::handleLeaveGame(RecievedMessage* msg)
{
	Game* temp = msg->getUser()->getGame();
	bool result = msg->getUser()->LeaveGame();	
	if (result)
	{
		try { delete temp; } catch (...) {}
		msg->getUser()->setGame(nullptr);
	}
}
void TriviaServer::handleStartGame(RecievedMessage* msg)
{
	Room* tempRoom = msg->getUser()->getRoom();
	try
	{
		msg->getUser()->setGame(new Game(tempRoom->getUsers(), tempRoom->getQuestionsNo(), _db));
	}
	catch (...)
	{
		Helper::sendData(msg->getSock(), P_CREATE_GAME_FAILED);
		return;
	}
	_roomsList.erase(tempRoom->getId());
	msg->getUser()->getGame()->sendFirstQuestion();
}
void TriviaServer::handlePlayerAnswer(RecievedMessage* msg)
{
	Game* temp = msg->getUser()->getGame();
	if (temp == nullptr)
		return;
	int answer = stoi(msg->getValues()[0]);
	int time = stoi(msg->getValues()[1]);
	bool result = temp->handleAnswerFromUser(msg->getUser(), answer, time);
	if (!result)
	{
		try { delete temp; } catch (...) {}
	}
}

bool TriviaServer::handleCreateRoom(RecievedMessage* msg) 
{
	if (msg->getUser() == nullptr)
		return false;
	string roomName = msg->getValues()[0];
	int numPlayers = stoi(msg->getValues()[1]);
	int numQuestions = stoi(msg->getValues()[2]);
	int time = stoi(msg->getValues()[3]);

	if (!msg->getUser()->CreateRoom(_roomIdSequence, roomName, numPlayers, numQuestions, time))
		return false;

	_roomsList.insert(pair<int, Room*>(_roomIdSequence, msg->getUser()->getRoom()));
	_roomIdSequence++;
	return true;
}
bool TriviaServer::handleCloseRoom(RecievedMessage* msg)
{
	if (msg->getUser()->getRoom() == nullptr)
		return false; 
	int id = msg->getUser()->getRoom()->getId();
	if (msg->getUser()->closeRoom() == -1)
		return false;
	_roomsList.erase(id);
	return true;
}
bool TriviaServer::handleJoinRoom(RecievedMessage* msg)
{
	if (msg->getUser() == nullptr)
		return false; 
	int id = stoi(msg->getValues()[0]);
	Room* roomToJoin = getRoomById(id);
	if (roomToJoin == nullptr)
		return false;
	return msg->getUser()->joinRoom(roomToJoin);
}
bool TriviaServer::handleLeaveRoom(RecievedMessage* msg)
{ 
	if (msg->getUser() == nullptr)
		return false;
	if (msg->getUser()->getRoom() == nullptr)
		return false;
	msg->getUser()->leaveRoom();
	return true;
}
void TriviaServer::handleGetUserInRoom(RecievedMessage* msg)
{
	if (msg->getUser() == nullptr)
		return;
	int id = stoi(msg->getValues()[0]);
	Room* roomToLook = getRoomById(id);
	if (roomToLook == nullptr)
		return;
	Helper::sendData(msg->getSock(), roomToLook->getUserListMessage());
}
void TriviaServer::handleGetRooms(RecievedMessage* msg)
{
	string message = P_SEND_ROOMS;
	message += Helper::getPaddedNumber(_roomsList.size(), 4);
	for (map<int, Room*>::iterator irt = _roomsList.begin(); irt != _roomsList.end(); irt++)
	{
		message += Helper::getPaddedNumber(irt->first, 4);
		message += Helper::getPaddedNumber(irt->second->getName().size(), 2);
		message += irt->second->getName();
	}
	Helper::sendData(msg->getSock(), message);
}
void TriviaServer::handleGetBestScores(RecievedMessage* msg)
{
	vector<string> bestScores = _db.getBestScores();
	string message = P_BEST_SCORE;
	for (int i = 0; i < bestScores.size(); i++)
		message += bestScores[i];
	Helper::sendData(msg->getSock(), message);
}

void TriviaServer::handleGetPersonalStatus(RecievedMessage* msg)
{
	string message = P_STATUS + _db.getPersonalStatus(msg->getUser()->getUsername());
	Helper::sendData(msg->getSock(), message);
}

void TriviaServer::handleRecievedMessage()
{
	srand(time(NULL));
	while (true)
	{
		unique_lock<mutex> lock(_mtxRecievedMessages);
		_cv.wait(lock, []{return true; });
		if (_queRcvMessages.empty()) // will probably never be true because of condition_variable 
			continue;
		RecievedMessage* temp = _queRcvMessages.front();
		_queRcvMessages.pop();
		switch (temp->getMessageCode())
		{
		case REQ_SIGN_IN:
			handleSignIn(temp);
			break;
		case REQ_SIGN_OUT:
			handleSignOut(temp);
			break;
		case REQ_SIGN_UP:
			handleSignUp(temp);
			break;
		case REQ_GET_ROOM_LIST:
			handleGetRooms(temp);
			break;
		case REQ_GET_USERS_IN_ROOM:
			handleGetUserInRoom(temp);
			break;
		case REQ_JOIN_ROOM:
			handleJoinRoom(temp);
			break;
		case REQ_LEAVE_ROOM:
			handleLeaveRoom(temp);
			break;
		case REQ_CREATE_ROOM:
			handleCreateRoom(temp);
			break;
		case REQ_CLOSE_ROOM:
			handleCloseRoom(temp);
			break;
		case REQ_START_GAME:
			handleStartGame(temp);
			break;
		case REQ_SENDING_ANSWER:
			handlePlayerAnswer(temp);
			break;
		case REQ_LEAVE_GAME:
			handleLeaveGame(temp);
			break;
		case REQ_BEST_SCORES:
			handleGetBestScores(temp);
			break;
		case REQ_MY_STATUS:
			handleGetPersonalStatus(temp);
			break;
		case REQ_EXIT_APP:
			safeDeleteUser(temp);
			break;
		}
		lock.unlock();
	}


}
void TriviaServer::addRecievedMessage(RecievedMessage* msg)
{
	{
		lock_guard<mutex> lock(_mtxRecievedMessages);
		_queRcvMessages.push(msg);
	} //in {} in order to free the lock after we push the data
	_cv.notify_one();
}
RecievedMessage* TriviaServer::buildRecieveMessage(SOCKET sock, int msgCode)
{
	RecievedMessage* temp;
	vector<string>* input = new vector<string>();
	switch (msgCode)
	{
	case REQ_SIGN_UP:
		pushFromSock(sock, input);
	case REQ_SIGN_IN:
		pushFromSock(sock, input);
		pushFromSock(sock, input);
		break;
	case REQ_GET_USERS_IN_ROOM:
	case REQ_JOIN_ROOM:
		input->push_back(Helper::getStringPartFromSocket(sock, 4));
		break;
	case REQ_CREATE_ROOM:
		pushFromSock(sock, input);
		input->push_back(Helper::getStringPartFromSocket(sock, 1));
		input->push_back(Helper::getStringPartFromSocket(sock, 2));
		input->push_back(Helper::getStringPartFromSocket(sock, 2));
		break;
	case REQ_SENDING_ANSWER:
		input->push_back(Helper::getStringPartFromSocket(sock, 1));
		input->push_back(Helper::getStringPartFromSocket(sock, 2));
		break;
	}
	if (input->empty())
	{
		temp = new RecievedMessage(sock, msgCode);
		delete input;
	}
	else
		temp = new RecievedMessage(sock, msgCode, *input);
	temp->setUser(getUserBySocket(sock));
	return temp;
}

User* TriviaServer::getUserByName(string name)
{
	for (map<SOCKET, User*>::iterator irt = _connectedUsers.begin(); irt != _connectedUsers.end(); irt++)
	{
		if (irt->second->getUsername() == name)
			return irt->second;
	}
	return nullptr;
}
User* TriviaServer::getUserBySocket(SOCKET sock)
{
	if (_connectedUsers.find(sock) != _connectedUsers.end())
		return _connectedUsers[sock];
	return nullptr;
}
Room* TriviaServer::getRoomById(int id)
{
	if (_roomsList.find(id) != _roomsList.end())
		return _roomsList[id];
	return nullptr;
}

void pushFromSock(SOCKET sock, vector<string>* vec, int length)
{
	vec->push_back(Helper::getStringPartFromSocket(sock, Helper::getIntPartFromSocket(sock, length)));
}
