#include "../UDPSocket.hpp"

UDPSocket::UDPSocket(char* ip, int port)
{
	this->port = port;
	this->ip = ip;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		handle_error("Unable to create socket");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	//servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(port);
	// Convert IPv4 and IPv6 addresses from text to binary form, if the conversion fails, the ip format is not correct
	/*if (inet_pton(AF_INET, ip, &servAddr.sin_addr) <= 0)
	{
		handle_error("Invalid address/ Address not supported");
	}*/
	int opt = 1;
	if (setsockopt(fd ,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) == -1) 	
	{
		perror("setsockopt"); exit(1);
	}
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd, (const struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if(port == 0)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		if(getsockname(fd, (struct sockaddr*)&addr, &len) < 0)
		{
			perror("Unable to get sock name");
		}
		else 
		{
			std::cout<<"UDP created on port: "<<addr.sin_port<<std::endl;
			servAddr.sin_port =this->port;
			this->port = addr.sin_port;
		}		
	}
/* 	servAddr.sin_addr.s_addr = inet_addr(ip);
	if (connect(fd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		handle_error("Connection Failed");
	} */
}

struct sockaddr_in UDPSocket::GetSockAddr()
{
	struct sockaddr_in addr;
	addr.sin_port = 8080;
	addr.sin_addr.s_addr = inet_addr(ip);
	return addr;
}

UDPSocket::~UDPSocket()
{
	close(fd);
}