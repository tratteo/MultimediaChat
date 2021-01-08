#pragma once
#include <string>
#include <iostream>
#define PAYLOAD_MSG 1
#define PAYLOAD_CREDENTIALS 2
#define PAYLOAD_DISCONNECT 3

struct MessagePayload
{
	std::string dstUsr;
	std::string message;
	int dstUsrLen;
	int messageLen;
	int size;

	void Create(std::string dstUsr, std::string message)
	{
		this->dstUsr = dstUsr;
		this->message = message;
		this->dstUsrLen = dstUsr.length();
		this->messageLen = message.length();
		this->size = dstUsrLen + messageLen + 8;
	}

	void Deserialize(char* payload)
	{
		dstUsrLen = 0x0;
		messageLen = 0x0;

		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			dstUsrLen |= (payload[offset++] << (8 * (i - 1)));
		}

		for (int i = 0; i < 4; i++)
		{
			messageLen |= (payload[offset++] << (8 * (i - 1)));
		}

		std::cout << "USRL: " << dstUsrLen << ", MSGL: " << messageLen << std::endl;
		dstUsr = "";
		message = "";

		for (int i = 0; i < dstUsrLen; i++)
		{
			dstUsr += payload[offset++];
		}
		for (int i = 0; i < messageLen; i++)
		{
			message += payload[offset++];
		}
		size = dstUsrLen + messageLen + 8;
	}

	char* Serialize()
	{
		char *buffer = new char[size];
		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			buffer[offset++] = (char)(dstUsrLen >> (8 * (i - 1)));
		}
		for (int i = 4; i < 8; i++)
		{
			buffer[offset++] = (char)(messageLen >> (8 * (i - 1)));
		}
		for (int i = 0; i < dstUsrLen; i++)
		{
			buffer[offset++] = dstUsr.at(i);
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

		std::cout << "USRL: " << usernameLen << ", MSGL: " << passwordLen << std::endl;
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

