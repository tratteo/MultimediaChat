#pragma once
#include <list>
#include "UserData.hpp"
#include "../include/ClientSessionData.hpp"
#include "Serializer.hpp"
#include <mutex>
#include <algorithm>

class DataBaseHandler
{
	public:
	DataBaseHandler();
	~DataBaseHandler();
	void UserConnected(ClientSessionData* data);
	void UserDisconnected(ClientSessionData* data);
	ClientSessionData* GetUserSession(std::string username);

	UserData* GetRegisteredUser(std::string username);
	void RegisterUser(UserData* user);
	bool IsUserRegistered(std::string username);
	private:
	std::mutex mutex;
	std::list<ClientSessionData*> connectedUsers;
	std::list<UserData> registeredUsers;
	Serializer* usersSer;
};