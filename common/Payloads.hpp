#pragma once
#include <string>
#include <iostream>
#include <ByteBufferUtils.hpp>

#define PAYLOAD_MSG 1
#define PAYLOAD_CREDENTIALS 2
#define PAYLOAD_DISCONNECT 3
#define PAYLOAD_LOGGED_IN 4
#define PAYLOAD_INVALID_CREDENTIALS 5
#define PAYLOAD_REGISTERED 6
#define PAYLOAD_INEXISTENT_DEST 7
#define	PAYLOAD_OFFLINE_USR 8
#define PAYLOAD_AUDIO_HEADER 9
#define PAYLOAD_ACK 10
#define PAYLOAD_DED_DGRAM_PORT 11
#define PAYLOAD_USER 12


class Payload
{
	public:
	inline int Size() const { return size; }

	protected:
	int size;
	Payload();
	Payload(int size);
	void Create(int size);
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
	MessagePayload(char* payload);
	MessagePayload(std::string from, std::string to, std::string message);
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
	CredentialsPayload(std::string username, std::string password);
	CredentialsPayload(char* payload);
	void Create(std::string username, std::string password);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
	inline std::string Username() const { return username; }
	inline std::string Password() const { return password; }
};

class AudioMessageHeaderPayload : public Payload
{
	private:
	std::string from;
	std::string to;
	int fromLen;
	int toLen;
	int segments;
	int messageLength;

	public:
	AudioMessageHeaderPayload(std::string from, std::string to, int segments, int messageLength);
	AudioMessageHeaderPayload(char* payload);
	void Create(std::string from, std::string to, int segments, int messageLength);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
	inline std::string From() const { return from; }
	inline std::string To() const { return to; }
	inline int Segments() const { return segments; }
	inline int TotalLength() const { return messageLength; }
};

class DgramPortPayload : public Payload
{
	private:
	int port;

	public:
	DgramPortPayload(int port);
	DgramPortPayload(char* payload);
	void Create(int port);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
	inline int Port() const { return port; }
};

class UserPayload : public Payload
{
	private:
	int usernameLen;
	std::string username;

	public:
	UserPayload(std::string username);
	UserPayload(char* payload);
	void Create(std::string username);
	void Deserialize(char* payload);
	char* Serialize() const;
	std::string ToString() const;
	inline std::string Username() const { return username; }
};
//class DgramAudioPayload : public Payload
//{
//	private:
//	int index;
//
//	public:
//	void Create(int index);
//	void Deserialize(char* payload);
//	char* Serialize() const;
//	std::string ToString() const;
//	inline std::string Index() const { return index; }
//};

