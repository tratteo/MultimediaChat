#pragma once
#include <iostream>
#include "../packets/Packet.cpp"
#define PACKET_CREDENTIALS 2

class CredentialsPacket : public Packet
{
	public:

	CredentialsPacket(char* packetByteBuf) : Packet(packetByteBuf)
	{
		int usrLength = 0x0;
		int offset = 0;
		for (int i = 0; i < 4; i++)
		{
			usrLength |= (data[offset++] << (8 * (i - 1)));
		}
		int passwordLength = 0x0;
		for (int i = 0; i < 4; i++)
		{
			passwordLength |= (data[offset++] << (8 * (i - 1)));
		}
		std::cout << "USRL: " << usrLength << ", PSWL: " << passwordLength << std::endl;
		username = "";
		password = "";
		for (int i = 0; i < usrLength; i++)
		{
			username += data[offset++];
		}
		for (int i = 0; i < passwordLength; i++)
		{
			password += data[offset++];
		}
	}

	CredentialsPacket(std::string username, std::string password) : Packet(PACKET_CREDENTIALS)
	{
		this->username = username;
		this->password = password;

		int usrLen = strlen(username.c_str());
		int msgLen = strlen(password.c_str());
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
			data[offset++] = username.at(i);
		}
		for (int i = 0; i < msgLen; i++)
		{
			data[offset++] = password.at(i);
		}
	}


	std::string GetUsername() { return username; }
	std::string GetPassword() { return password; }

	private:
	std::string username;
	std::string password;
};