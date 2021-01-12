#pragma once
#include <list>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <regex>
#include "UserData.hpp"
#include "../include/ClientSessionData.hpp"
#include "../include/ChatData.hpp"
#include "Serializer.hpp"

#define DATABASE_PATH "data"

class DataBaseHandler
{
	public:
	std::vector<std::string> GetFilenames(std::string path);
	DataBaseHandler();
	~DataBaseHandler();
	void UserConnected(ClientSessionData* data);
	void UserDisconnected(ClientSessionData* data);
	ClientSessionData* GetUserSession(std::string username);

	void SerializeDatabase();
	void ParseDatabase();
	void AddMessage(MessagePayload message);
	UserData* GetRegisteredUser(std::string username);
	void RegisterUser(UserData* user);
	bool IsUserRegistered(std::string username);
	private:
	bool ChatDataExists(std::string path);
	std::mutex mutex;
	std::list<ClientSessionData*> connectedUsers;
	std::list<UserData*> registeredUsers;
};