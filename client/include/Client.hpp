#pragma once
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <cmath>
#include <atomic>
#include <poll.h>
#include "../../common/Poller.hpp"
#include "../../common/CSocket.hpp"
#include "../../common/SoundRegistrer.hpp"
#include "../../common/NetworkHandler.hpp"
#include "../../common/Packet.hpp"
#include "../../common/Payloads.hpp"
#include "../../common/ByteBufferUtils.hpp"
#include "../../common/UDPSocket.hpp"
#define STDIN_IDX 0
#define TCP_IDX 1
#define UDP_IDX 2
#define POLL_DELAY 150
#define POLLED_SIZE 3
#define BUF_SIZE 512
#define REGISTER_KEY "/r"
#define QUIT_KEY "/q"
#define CHANGE_DEST_KEY "/c"

class Client
{
    public:
    Client(char* servIp);
    ~Client();
    void Run();
    void Join();

    inline CSocket *GetTCP() const { return clientSocket; } 
    inline UDPSocket* GetUDP() const {return udpSocket;}
    inline bool IsLogged() const {return logged;}

    private:
    void Loop();
    void LoginRoutine();
    void ReceiveAudio(AudioMessageHeaderPayload header);
    void ReceiveDaemon();
    void SendAudio(std::string dest);

    struct pollfd polledFds[POLLED_SIZE];
    std::thread loopThread;
	std::string currentDest;
    int udpPort;
    bool logged = false;
    std::atomic<bool> shutDown;
    CSocket* clientSocket;
    UDPSocket* udpSocket;

    std::thread receiveDaemon;
    std::thread audioRecvThread;
    std::thread loginThread;
    std::string username;
    std::list<bool*> acks;
    uint8_t lastReceived;
    char* serv_ip;
};