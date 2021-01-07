#pragma once
#include <iostream>
#include "../packets/Packet.cpp"
#define PACKET_MSG 1

class MessagePacket : public Packet
{
	public:

	MessagePacket(char* packetByteBuf) : Packet(packetByteBuf)
	{
		int usrLength = 0x0;
		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			usrLength |= (data[offset++] << (8 * (i - 1)));
		}
		int msgLen = 0x0;
		for (int i = 0; i < 4; i++)
		{
			msgLen |= (data[offset++] << (8 * (i - 1)));
		} 
		std::cout << "USRL: " << usrLength << ", MSGL: " << msgLen << std::endl;
		destUsr = "";
		message = "";
		for (int i = 0; i < usrLength; i++)
		{
			destUsr += data[offset++];
		}
		for (int i = 0; i < msgLen; i++)
		{
			message += data[offset++];
		}
	}

	MessagePacket(std::string destUser, std::string message) : Packet(PACKET_MSG)
	{
		this->destUsr = destUser;
		this->message = message;

		int usrLen = strlen(destUser.c_str());
		int msgLen = strlen(message.c_str());
		length = usrLen + msgLen + 8;

		data = new char[length];
		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			data[offset++] = (char)(usrLen >> (8 * (i - 1)));
		}
		for (int i = 4; i < 8; i++)
		{
			data[offset++] = (char)(msgLen >> (8 * (i - 1)));
		}
		for (int i = 0; i < usrLen; i++)
		{
			data[offset++] = destUser.at(i);
		}
		for (int i = 0; i < msgLen; i++)
		{
			data[offset++] = message.at(i);
		}
	}


	std::string GetUsr() { return destUsr; }
	std::string GetMessage() { return message; }

	private:
	std::string destUsr;
	std::string message;
};