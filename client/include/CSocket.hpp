#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "../../common/errhandler.hpp"
#define PORT 8080

class CSocket
{
	private:
	bool connected;
	char* serverIp;
	int socketFd;
	struct sockaddr_in servAddr;


	public:

	~CSocket();
	CSocket(char* serverIp);
	void Init(int type, int protocol);
	int GetFd();
	void TryConnect();
	bool IsConnected();
	
};

