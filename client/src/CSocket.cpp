#include "../include/CSocket.hpp"

CSocket::CSocket(char* server_ip)
{
	connected = false;
	this->serverIp = server_ip;
}

int CSocket::GetFd()
{
	return socketFd;
}

void CSocket::Init(int type, int protocol)
{
	if ((socketFd = socket(AF_INET, type, protocol)) < 0)
	{
		handle_error("Unable to create socket");
	}
	
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form, if the conversion fails, the ip format is not correct
	if (inet_pton(AF_INET, serverIp, &servAddr.sin_addr) <= 0)
	{
		handle_error("Invalid address/ Address not supported");
	}
}

void CSocket::TryConnect()
{
	if (connect(socketFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		handle_error("Connection Failed");
	}
	else
	{
		connected = true;
	}
}

bool CSocket::IsConnected()
{
	return connected;
}

CSocket::~CSocket()
{
	std::cout<<"Calling csocket destructor"<<std::endl;
	int ret = close(socketFd);
	if (ret < 0)
	{
		handle_error("Unable to close socket");
	}
}
