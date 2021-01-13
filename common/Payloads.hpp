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

class Payload
{
	public:
	inline int Size() const { return size; }

	protected:
	int size;
	Payload();
	Payload(int size);
	void Create(int size);

	void PutString(char* startAddress, std::string value) const;
	void PutInt(char* startAddress, int value) const;
	int ReadInt(char* currentAddress) const;
	std::string ReadString(char* currentAddress, int length) const;
};

class MessagePayload : public Payload
{
	private:
	std::string from;
	std::string to;
	std::string message;
	int fromLen;
	int toLen;
	int messageLen;

	public:
	void Create(std::string from, std::string to, std::string message);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
	inline std::string From() const { return from; }
	inline std::string To() const { return to; }
	inline std::string Message() const { return message; }
};

class CredentialsPayload : public Payload
{
	private:
	std::string username;
	std::string password;
	int usernameLen;
	int passwordLen;

	public:
	void Create(std::string username, std::string password);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
	inline std::string Username() const { return username; }
	inline std::string Password() const { return password; }
};

