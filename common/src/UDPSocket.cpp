#include "../UDPSocket.hpp"

UDPSocket::UDPSocket(char* ip, int port, Type type)
{
	this->port = port;
	this->ip = ip;

	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		handle_error("Unable to create socket");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	// Convert IPv4 and IPv6 addresses from text to binary form, if the conversion fails, the ip format is not correct
	/*if (inet_pton(AF_INET, ip, &servAddr.sin_addr) <= 0)
	{
		handle_error("Invalid address/ Address not supported");
	}*/

	switch (type)
	{
		case UDPSocket::IN:
			servAddr.sin_addr.s_addr = INADDR_ANY;
			if (bind(fd, (const struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
			{
				perror("bind failed");
				exit(EXIT_FAILURE);
			}
			break;
		case UDPSocket::OUT:
			servAddr.sin_addr.s_addr = inet_addr(ip);
			break;
		default:
			break;
	}
}
UDPSocket::~UDPSocket()
{
	close(fd);
}