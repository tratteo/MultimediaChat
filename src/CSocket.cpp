#include "CSocket.hpp"

CSocket::CSocket(char* server_ip)
{
	connected = false;
	this->server_ip = server_ip;
}

void CSocket::init(int type, int protocol)
{
	if ((socket_fd = socket(AF_INET, type, protocol)) < 0)
	{
		handle_error("Unable to create socket");
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form, if the conversion fails, the ip format is not correct
	if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
	{
		handle_error("Invalid address/ Address not supported");
	}
}

void CSocket::try_connect()
{
	if (connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		handle_error("Connection Failed");
	}
	else
	{
		connected = true;
	}
}

bool CSocket::is_connected()
{
	return connected;
}

CSocket::~CSocket()
{
	int ret = close(socket_fd);
	if (ret < 0)
	{
		handle_error("Unable to close socket");
	}
}
