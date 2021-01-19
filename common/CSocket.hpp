#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "errhandler.hpp"
#include "ClientSessionData.hpp"

class ClientSessionData;

class CSocket
{
	enum Type
	{
		SERVER,
		CLIENT
	};

	private:
	bool connected = false;
	char* serverIp;
	int socketFd;
	Type type = Type::CLIENT;
	struct sockaddr_in servAddr;
	int port;
	void Init();

	public:
	~CSocket();
	CSocket(int port);
	CSocket(char* server_ip, int port);

	ClientSessionData* AcceptConnection() const;
	inline const struct sockaddr_in* GetServAddr() const { return &servAddr; }
	inline char* GetIp() const { return serverIp; }
	int GetFd() const;
	bool IsConnected() const;

	bool TryConnect();

	
};

