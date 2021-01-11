#pragma once
#include <list>
#include "UserData.hpp"
#include "../include/ClientSessionData.hpp"
#include "../../common/Chat.cpp"
#include "Serializer.hpp"
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <regex>

#define DATABASE_PATH "../data"

class DataBaseHandler
{
	public:
	std::vector<std::string> GetFilenames(std::string path);
	DataBaseHandler();
	~DataBaseHandler();
	void UserConnected(ClientSessionData* data);
	void UserDisconnected(ClientSessionData* data);
	ClientSessionData* GetUserSession(std::string username);

	void ParseDatabase();
	UserData* GetRegisteredUser(std::string username);
	void RegisterUser(UserData* user);
	bool IsUserRegistered(std::string username);
	private:
	bool ChatDataExists(std::string path);
	std::list<Chat*> chats;
	std::mutex mutex;
	std::list<ClientSessionData*> connectedUsers;
	std::list<UserData> registeredUsers;
};