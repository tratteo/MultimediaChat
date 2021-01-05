#pragma once
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
#include "../include/ClientSessionData.hpp"

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

	ClientSessionData* AcceptConnection();
	void init(int type, int protocol);

	//Getters
	bool isConnected();
	sockaddr_in getClientAddress();
	int getServerSocket();
};