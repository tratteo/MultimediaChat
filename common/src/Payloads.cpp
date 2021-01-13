#include "../Payloads.hpp"

Payload::Payload(int size)
{
	this->size = size;
}

Payload::Payload()
{

}

void Payload::Create(int size)
{
	this->size = size;
}

void Payload::PutString(char* startAddress, std::string value) const
{
	int len = value.size();
	for (int i = 0; i < len; i++)
	{
		startAddress[i] = value.at(i);
	}
}

void Payload::PutInt(char* startAddress, int value) const
{
	int size = sizeof(int);
	for (int i = 0; i < size; i++)
	{
		startAddress[i] = (char)(value >> (sizeof(char) * (i - 1)));
	}
}

int Payload::ReadInt(char* currentAddress) const
{
	int res = 0x0;
	int size = sizeof(int);
	for (int i = 0; i < size; i++)
	{
		res |= (currentAddress[i] << (sizeof(char) * (i - 1)));
	}
	return res;
}

std::string Payload::ReadString(char* currentAddress, int length) const
{
	std::string res = "";
	for (int i = 0; i < length; i++)
	{
		res += currentAddress[i];
	}
	return res;
}

void MessagePayload::Create(std::string from, std::string to, std::string message)
{
	this->from = from;
	this->to = to;
	this->message = message;
	this->fromLen = from.length();
	this->toLen = to.length();
	this->messageLen = message.length();
	Payload::Create(fromLen + toLen + messageLen + 3 * sizeof(int));
}

void MessagePayload::Deserialize(char* payload)
{
	int offset = 0;
	fromLen = ReadInt(payload + offset);
	offset += sizeof(int);

	toLen = ReadInt(payload + offset);
	offset += sizeof(int);

	messageLen = ReadInt(payload + offset);
	offset += sizeof(int);

	from = ReadString(payload + offset, fromLen);
	offset += fromLen;

	to = ReadString(payload + offset, toLen);
	offset += toLen;

	message = ReadString(payload + offset, messageLen);
	offset += messageLen;

	Payload::Create(offset);
}

char* MessagePayload::Serialize() const
{
	char* buffer = new char[size];

	int offset = 0;

	PutInt(buffer + offset, fromLen);
	offset += sizeof(int);

	PutInt(buffer + offset, toLen);
	offset += sizeof(int);

	PutInt(buffer + offset, messageLen);
	offset += sizeof(int);

	PutString(buffer + offset, from);
	offset += fromLen;

	PutString(buffer + offset, to);
	offset += toLen;

	PutString(buffer + offset, message);
	offset += messageLen;

	return buffer;
}

std::string MessagePayload::ToString() const
{
	return from + ">" + to + ":" + message + "\n";
}


void CredentialsPayload::Create(std::string username, std::string password)
{
	this->username = username;
	this->password = password;
	this->usernameLen = username.length();
	this->passwordLen = password.length();
	Payload::Create(usernameLen + passwordLen + 2 * sizeof(int));
}

void CredentialsPayload::Deserialize(char* payload)
{
	int offset = 0;
	usernameLen = ReadInt(payload + offset);
	offset += sizeof(int);

	passwordLen = ReadInt(payload + offset);
	offset += sizeof(int);

	username = ReadString(payload + offset, usernameLen);
	offset += usernameLen;

	password = ReadString(payload + offset, passwordLen);
	offset += passwordLen;

	Payload::Create(offset);
}

char* CredentialsPayload::Serialize() const
{
	char* buffer = new char[size];
	int offset = 0;

	PutInt(buffer + offset, usernameLen);
	offset += sizeof(int);

	PutInt(buffer + offset, passwordLen);
	offset += sizeof(int);

	PutString(buffer + offset, username);
	offset += usernameLen;

	PutString(buffer + offset, password);
	offset += passwordLen;

	return buffer;
}

std::string CredentialsPayload::ToString() const
{
	return username + "-" + password + "\n";
}


