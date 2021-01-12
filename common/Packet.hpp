#pragma once
#include <iostream>
#include <cstring>

class Packet
{
	public:
	~Packet();
	Packet();
	Packet(char type, char* payload, int payloadLength);
	Packet(char* packetByteBuf);

	char* Serialize() const;
	void FromData(char type, char* payload, int payloadLength);
	void FromByteBuf(char* packetByteBuf);
	void Create(char type);
	// Getters
	char GetType() const;
	int GetLength() const;
	int GetTotalLength() const;
	char* GetData() const;

	private:
	char type;
	int length;
	char* data;
};