#include <iostream>
#include <cstring>

class Packet
{
	public:

	~Packet();
	Packet();
	Packet(char type, char* payload, int payloadLength);

	Packet(char* packetByteBuf);

	char* Serialize();
	void FromData(char type, char* payload, int payloadLength);
	void FromByteBuf(char* packetByteBuf);
	void Create(char type);

	// Getters
	char GetType();
	int GetLength();
	int GetTotalLength();
	char* GetData();

	protected:
	char type;
	int length;
	char* data;

};