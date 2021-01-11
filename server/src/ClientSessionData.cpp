#include "../include/ClientSessionData.hpp"

ClientSessionData::~ClientSessionData()
{
}

ClientSessionData::ClientSessionData(int fd, char* ip)
{
    this->fd = fd;
    this->ip = ip;
    this->logged = false;
    this->owner = nullptr;
}

void ClientSessionData::RegisterOwner(UserData* owner)
{
    if (this->owner != nullptr)
    {
        delete owner;

    }
    this->owner = owner;
}   

UserData* ClientSessionData::GetOwner() const
{
    return this->owner;
}

int ClientSessionData::GetFd() const
{
    return this->fd;
}

char* ClientSessionData::GetIp() const
{
    return this->ip;
}