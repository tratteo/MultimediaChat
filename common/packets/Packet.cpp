#include <iostream>
#include <string.h>

class Packet
{
	public:

	Packet(char id)
	{
		this->id = id;
	}

	Packet(char* packetByteBuf)
	{
		id = packetByteBuf[0];
		length = 0x0;
		for (int i = 1; i < 5; i++)
		{
			length |= (packetByteBuf[i] << (8 * (i - 1)));
		}
		data = new char[length];
		memcpy(data, packetByteBuf + 5, length);
	}

	char* Serialize()
	{
		char* packetByteBuf = new char[length + 5];
		packetByteBuf[0] = id;
		
		for (int i = 1; i < 5; i++)
		{
			packetByteBuf[i] = (char)(length >> (8 * (i - 1)));
		}
		memcpy(packetByteBuf + 5, data, length);
		return packetByteBuf;
	}

	// Getters
	char GetId() { return id; }
	int GetLength() { return length; }
	int GetTotalLength() { return length + 5; }

	protected:
	char id;
	int length;
	char* data;

};