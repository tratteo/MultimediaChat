#include "../CSocket.hpp"

CSocket::CSocket(char* server_ip, int port)
{
	type = CSocket::CLIENT;
	connected = false;
	this->port = port;
	this->serverIp = server_ip;

	Init();

	// Convert IPv4 and IPv6 addresses from text to binary form, if the conversion fails, the ip format is not correct
	if (inet_pton(AF_INET, server_ip, &servAddr.sin_addr) <= 0)
	{
		handle_error("Invalid address/ Address not supported");
	}
}

void CSocket::Init()
{
	if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		handle_error("Unable to create socket");
	}

	int opt = 1;
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handle_fatal_error("Unable to setsockopt");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
}

CSocket::CSocket(int port)
{
	type == CSocket::SERVER;
	connected = false;
	this->port = port;

	Init();

	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Forcefully attaching socket to the port 
	if (bind(socketFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		handle_fatal_error("Unable to bind the server socket");
	}

	if (listen(socketFd, 5) < 0)
	{
		handle_fatal_error("Unable to listen to the server socket");
	}

}

int CSocket::GetFd() const
{
	return socketFd;
}

ClientSessionData* CSocket::AcceptConnection() const
{
	if (type != CSocket::SERVER)
	{
		std::cout << "Cannot call accept on client initialized socket" << std::endl;
		return nullptr;
	}
	int socket_fd;
	int addrlen = sizeof(servAddr);
	if ((socket_fd = accept(socketFd, (struct sockaddr*)&servAddr, (socklen_t*)&addrlen)) < 0)
	{
		handle_error("Unable to connect");
		return nullptr;
	}
	else
	{
		char* ip = inet_ntoa(servAddr.sin_addr);
		ClientSessionData* data = new ClientSessionData(socket_fd, ip);
		return data;    UDPSocket* outUdpSocket;
	}
}

bool CSocket::TryConnect()
{
	if (type != CSocket::CLIENT)
	{
		std::cout << "Cannot call connect on server initialized socket" << std::endl;
		return false;
	}

	if (connect(socketFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		handle_error("Connection Failed");
		return false;
	}
	else
	{
		connected = true;
		return true;
	}
}

bool CSocket::IsConnected() const
{
	return connected;
}

CSocket::~CSocket()
{
	if (close(socketFd) < 0)
	{
		handle_error("Unable to close server socket fd");
	}
}
