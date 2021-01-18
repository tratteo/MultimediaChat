#pragma once
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include <cmath>
#include "../../common/CSocket.hpp"
#include "../../common/SoundRegistrer.hpp"
#include "../../common/NetworkHandler.hpp"
#include "../../common/Packet.hpp"
#include "../../common/Payloads.hpp"
#include "../../common/ByteBufferUtils.hpp"
#include "../../common/UDPSocket.hpp"
#define BUF_SIZE 512
#define REGISTER_KEY "/r"
#define QUIT_KEY "/q"
#define CHANGE_DEST_KEY "/c"

class Client
{
    public:
    Client(char* servIp);
    ~Client();
    int Run();

    inline CSocket *GetTCP() const { return clientSocket; } 
    inline UDPSocket* GetUDP() const {return udpSocket;}
    inline bool IsLogged() const {return logged;}

    private:
    void LoginRoutine();
    void ReceiveAudio(AudioMessageHeaderPayload header);
    void ReceiveDaemon();
    void SendAudio(std::string dest);

	std::string currentDest;
    int udpPort;
    bool logged = false;
    bool shutDown = false;
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