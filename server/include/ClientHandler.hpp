#pragma once
#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <atomic>
#include <poll.h>
#include <string>
#include <list>
#include "../../common/Poller.hpp"
#include "../../common/Sound.hpp"
#include "../../common/UserData.hpp"
#include "../../common/errhandler.hpp" 
#include "../../common/ClientSessionData.hpp"
#include "../../common/Payloads.hpp"
#include "../../common/Packet.hpp"
#include "../../common/NetworkHandler.hpp"
#include "../include/DatabaseHandler.hpp"
#define TCP_IDX 0
#define UDP_IDX 1
#define POLL_DELAY 20
#define POLLED_SIZE 2
#define BUF_SIZE 512

class ClientHandler
{
    public:
    ClientHandler(ClientSessionData *sessionData, DataBaseHandler *dataHandler);
    ~ClientHandler();
    void HandleConnection();
    void CloseConnection();

    private:
    bool active = false;
    struct pollfd polledFds[POLLED_SIZE];
    void NotifyUDPPort();
    void UDPReceive(AudioMessageHeaderPayload amhPayload);
    ClientSessionData *sessionData;
    DataBaseHandler *dataHandler;
    std::atomic<bool> shutdownReq;
    bool closedLocal = true;
    std::thread mainThread;
    std::thread udpThread;
    void Loop();
};