#pragma once
#include <iostream>
#include <cstring>

class Packet
{
	public:
	~Packet();
	Packet();
	Packet(char type);
	Packet(char type, char* payload, int payloadLength);
	Packet(char* packetByteBuf);

	void Purge();
	char* Serialize() const;
	void FromData(char type, char* payload, int payloadLength);
	void FromByteBuf(char* packetByteBuf);
	void CreateTrivial(char type);
	inline char GetType() const { return type; };
	inline int GetLength() const { return length; };
	inline int GetTotalLength() const { return length + sizeof(int) + 1; };
	inline char* GetData() const { return data; };

	private:
	char type;
	int length;
	char* data = nullptr;
};