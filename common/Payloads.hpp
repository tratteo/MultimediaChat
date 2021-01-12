#pragma once
#include <string>
#include <iostream>

#define PAYLOAD_MSG 1
#define PAYLOAD_CREDENTIALS 2
#define PAYLOAD_DISCONNECT 3
#define PAYLOAD_LOGGED_IN 4
#define PAYLOAD_INVALID_CREDENTIALS 5
#define PAYLOAD_REGISTERED 6
#define PAYLOAD_INEXISTENT_DEST 7
#define	PAYLOAD_OFFLINE_USR 8

struct MessagePayload
{
	std::string from;
	std::string to;
	std::string message;
	int fromLen;
	int toLen;
	int messageLen;
	int size;

	void Create(std::string from, std::string to, std::string message);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
};

struct CredentialsPayload
{
	std::string username;
	std::string password;
	int usernameLen;
	int passwordLen;
	int size;

	void Create(std::string username, std::string password);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
};

