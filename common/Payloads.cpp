#pragma once
#include <string>
#include <iostream>
#define PAYLOAD_MSG 1
#define PAYLOAD_CREDENTIALS 2
#define PAYLOAD_DISCONNECT 3
#define PAYLOAD_LOGGED_IN 4
#define PAYLOAD_INVALID_CREDENTIALS 5
#define PAYLOAD_REGISTERED 6

struct MessagePayload
{
	std::string from;
	std::string to;
	std::string message;
	int fromLen;
	int toLen;
	int messageLen;
	int size;

	void Create(std::string from, std::string to, std::string message)
	{
		this->from = from;
		this->to = to;
		this->message = message;
		this->fromLen = from.length();
		this->toLen = to.length();
		this->messageLen = message.length();
		this->size = fromLen + toLen + messageLen + 12;
	}

	void Deserialize(char* payload)
	{
		fromLen = 0x0;
		toLen = 0x0;
		messageLen = 0x0;

		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			fromLen |= (payload[offset++] << (8 * (i - 1)));
		}

		for (int i = 0; i < 4; i++)
		{
			toLen |= (payload[offset++] << (8 * (i - 1)));
		}

		for (int i = 0; i < 4; i++)
		{
			messageLen |= (payload[offset++] << (8 * (i - 1)));
		}

		from = "";
		to = "";
		message = "";

		for (int i = 0; i < fromLen; i++)
		{
			from += payload[offset++];
		}

		for (int i = 0; i < toLen; i++)
		{
			to += payload[offset++];
		}

		for (int i = 0; i < messageLen; i++)
		{
			message += payload[offset++];
		}

		size = fromLen + toLen + messageLen + 12;
	}

	char* Serialize()
	{
		char *buffer = new char[size];
		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			buffer[offset++] = (char)(fromLen >> (8 * (i - 1)));
		}
		for (int i = 0; i < 4; i++)
		{
			buffer[offset++] = (char)(toLen >> (8 * (i - 1)));
		}
		for (int i = 0; i < 4; i++)
		{
			buffer[offset++] = (char)(messageLen >> (8 * (i - 1)));
		}
		for (int i = 0; i < fromLen; i++)
		{
			buffer[offset++] = from.at(i);
		}
		for (int i = 0; i < toLen; i++)
		{
			buffer[offset++] = to.at(i);
		}
		for (int i = 0; i < messageLen; i++)
		{
			buffer[offset++] = message.at(i);
		}

		return buffer;
	}
};

struct CredentialsPayload
{
	std::string username;
	std::string password;
	int usernameLen;
	int passwordLen;
	int size;

	void Create(std::string username, std::string password)
	{
		this->username = username;
		this->password = password;
		this->usernameLen = username.length();
		this->passwordLen = password.length();
		this->size = usernameLen + passwordLen + 8;
	}

	void Deserialize(char* payload)
	{
		usernameLen = 0x0;
		passwordLen = 0x0;

		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			usernameLen |= (payload[offset++] << (8 * (i - 1)));
		}

		for (int i = 0; i < 4; i++)
		{
			passwordLen |= (payload[offset++] << (8 * (i - 1)));
		}

		username = "";
		password = "";

		for (int i = 0; i < usernameLen; i++)
		{
			username += payload[offset++];
		}
		for (int i = 0; i < passwordLen; i++)
		{
			password += payload[offset++];
		}

		size = usernameLen + passwordLen + 8;
	}

	char* Serialize()
	{
		char* buffer = new char[size];
		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			buffer[offset++] = (char)(usernameLen >> (8 * (i - 1)));
		}
		for (int i = 4; i < 8; i++)
		{
			buffer[offset++] = (char)(passwordLen >> (8 * (i - 1)));
		}
		for (int i = 0; i < usernameLen; i++)
		{
			buffer[offset++] = username.at(i);
		}
		for (int i = 0; i < passwordLen; i++)
		{
			buffer[offset++] = password.at(i);
		}

		return buffer;
	}
};

