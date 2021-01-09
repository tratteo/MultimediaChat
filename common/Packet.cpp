#include <iostream>
#include <string.h>

class Packet
{
	public:

	~Packet()
	{
		delete data;
	}
	Packet()
	{

	}
	Packet(char type, char* payload, int payloadLength)
	{
		FromData(type, payload, payloadLength);
	}

	Packet(char* packetByteBuf)
	{
		FromByteBuf(packetByteBuf);
	}

	char* Serialize()
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

	void FromData(char type, char* payload, int payloadLength)
	{
		data = payload;
		this->type = type;
		this->length = payloadLength;
	}

	void FromByteBuf(char* packetByteBuf)
	{
		type = packetByteBuf[0];
		length = 0x0;
		for (int i = 1; i < 5; i++)
		{
			length |= (packetByteBuf[i] << (8 * (i - 1)));
		}
		data = new char[length];
		memcpy(data, packetByteBuf + 5, length);
	}

	void Create(char type)
	{
		this->type = type;
		data = nullptr;
		length = 0;
	}

	// Getters
	char GetType() { return type; }
	int GetLength() { return length; }
	int GetTotalLength() { return length + 5; }
	char* GetData() { return data; }

	protected:
	char type;
	int length;
	char* data;

};