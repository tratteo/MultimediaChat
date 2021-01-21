#include "../UDPSocket.hpp"

UDPSocket::UDPSocket(int port)
{
	this->port = port;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		handle_error("Unable to create socket");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);

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

	// Set the file descriptor as non blocking
}
UDPSocket::~UDPSocket()
{
	close(fd);
}