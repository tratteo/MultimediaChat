#pragma once
#include "UserData.hpp"
#include "Serializer.hpp"
#include <mutex>

class DataBaseHandler
{
	public:
	DataBaseHandler();
	~DataBaseHandler();

	void AddUser(UserData* user);
	bool IsUserRegistered(UserData* user);
	private:
	std::recursive_mutex mutex;
	Serializer* usersSer;
};