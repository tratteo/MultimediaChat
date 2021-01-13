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
	inline struct sockaddr_in GetServAddr() const { return servAddr; }
	int GetFd() const;
	void TryConnect();
	bool IsConnected() const;
	
};

