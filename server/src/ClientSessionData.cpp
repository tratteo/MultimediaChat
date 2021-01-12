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

bool ClientSessionData::IsLogged() const
{
    return logged;
}

void ClientSessionData::UserLogged(UserData* owner)
{
    if (this->owner != nullptr)
    {
        delete owner;

    }
    this->owner = owner;
    logged = true;
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