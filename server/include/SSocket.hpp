#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include "../../common/errhandler.hpp"

#define PORT 8080

class SSocket
{
    private:

	int server_fd;
    struct sockaddr_in address;
    int addrlen;

    public:

	SSocket();
	~SSocket();

	int AcceptConnection();
	void init(int type, int protocol);

	//Getters
	bool isConnected();
	sockaddr_in getClientAddress();
	int getServerSocket();

	//Setters
	void setConnected(bool);
};