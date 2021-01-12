#pragma once
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
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
	void Init(int type, int protocol);
};