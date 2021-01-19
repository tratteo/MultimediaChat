#include "../Packet.hpp"

Packet::~Packet()
{
/* 	if (data != nullptr)
	{
		delete[] data;
	} */
}

Packet::Packet()
{
	Purge();
}

Packet::Packet(char type)
{
	CreateTrivial(type);
}

Packet::Packet(char type, char* payload, int payloadLength)
{
	FromData(type, payload, payloadLength);
}

Packet::Packet(char* packetByteBuf)
{
	FromByteBuf(packetByteBuf);
}

char* Packet::Serialize() const
{
	char* packetByteBuf = new char[length + 5];
	packetByteBuf[0] = type;

	for (int i = 1; i < 5; i++)
	{
		packetByteBuf[i] = (char)(length >> (8 * (i - 1)));
	}
	memcpy(packetByteBuf + 5, data, length);
	return packetByteBuf;
}

void Packet::Purge()
{
	if(data != nullptr)
	{
		delete[] data;
		data = nullptr;
	}
	length = 0;
	type = -1;
}

void Packet::FromData(char type, char* payload, int payloadLength)
{
	Purge();
	data = new char[payloadLength];
	memcpy(data, payload, payloadLength);
	this->type = type;
	this->length = payloadLength;
}

void Packet::FromByteBuf(char* packetByteBuf)
{
	Purge();
	type = packetByteBuf[0];
	length = 0x0;
	for (int i = 1; i < 5; i++)
	{
		length |= (packetByteBuf[i] << (8 * (i - 1)));
	}
	data = new char[length];
	memcpy(data, packetByteBuf + 5, length);
}

void Packet::CreateTrivial(char type)
{
	Purge();
	this->type = type;
	data = nullptr;
	length = 0;
}
