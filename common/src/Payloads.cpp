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
	fromLen = ReadUInt(payload + offset);
	offset += sizeof(int);

	toLen = ReadUInt(payload + offset);
	offset += sizeof(int);

	messageLen = ReadUInt(payload + offset);
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

	PutUInt(buffer + offset, fromLen);
	offset += sizeof(int);

	PutUInt(buffer + offset, toLen);
	offset += sizeof(int);

	PutUInt(buffer + offset, messageLen);
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
	usernameLen = ReadUInt(payload + offset);
	offset += sizeof(int);

	passwordLen = ReadUInt(payload + offset);
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

	PutUInt(buffer + offset, usernameLen);
	offset += sizeof(int);

	PutUInt(buffer + offset, passwordLen);
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

void AudioMessageHeaderPayload::Create(std::string from, std::string to, int segments, int messageLength)
{
	this->from = from;
	this->to = to;
	this->fromLen = from.length();
	this->toLen = to.length();
	this->segments = segments;
	this->messageLength = messageLength;

	Payload::Create(fromLen + toLen + sizeof(int) * 4);
}
void AudioMessageHeaderPayload::Deserialize(char* payload)
{
	int offset = 0;

	fromLen = ReadUInt(payload + offset);
	offset += sizeof(int);

	toLen = ReadUInt(payload + offset);
	offset += sizeof(int);

	from = ReadString(payload + offset, fromLen);
	offset += fromLen;

	to = ReadString(payload + offset, toLen);
	offset += toLen;

	segments = ReadUInt(payload + offset);
	offset += sizeof(int);

	messageLength = ReadUInt(payload + offset);
	offset += sizeof(int);

	Payload::Create(offset);
}
char* AudioMessageHeaderPayload::Serialize() const
{
	char* buffer = new char[size];
	int offset = 0;

	PutUInt(buffer + offset, fromLen);
	offset += sizeof(int);

	PutUInt(buffer + offset, toLen);
	offset += sizeof(int);

	PutString(buffer + offset, from);
	offset += fromLen;

	PutString(buffer + offset, to);
	offset += toLen;

	PutUInt(buffer + offset, segments);
	offset += sizeof(int);

	PutUInt(buffer + offset, messageLength);
	offset += sizeof(int);

	return buffer;
	
}
std::string AudioMessageHeaderPayload::ToString() const
{
	return from + ">" + to + ", Voice msg of segs: " + std::to_string(segments) + ", length: " + std::to_string(messageLength);
}


void DgramPortPayload::Create(int port)
{
	this->port = port;
	Payload::Create(sizeof(int));
}
void DgramPortPayload::Deserialize(char* payload)
{
	int offset = 0;
	port = ReadUInt(payload + offset);
	offset += sizeof(int);
	Payload::Create(offset);
}
char* DgramPortPayload::Serialize() const
{
	char* buffer = new char[size];
	int offset = 0;
	PutUInt(buffer + offset, port);
	offset += sizeof(int);

	return buffer;
}
std::string DgramPortPayload::ToString() const
{
	return "Port: " + std::to_string(port);
}


//
//void DgramAudioPayload::Create(int index)
//{
//	this->index = index;
//	Payload::Create(sizeof(int));
//}
//void DgramAudioPayload::Deserialize(char* payload)
//{
//	int offset = 0;
//	index = ReadUInt(payload + offset);
//	offset += sizeof(int);
//}
//char* DgramAudioPayload::Serialize() const;
//std::string DgramAudioPayload::ToString() const;
