#pragma once
#include <string>
#include <list>
#include "../../common/Payloads.cpp"
#include "../../common/Chat.cpp"

class UserData
{

    private: 
    std::string username;
    std::string password;
    std::list<Chat> chats;

    public:
    UserData();
    UserData(std::string username, std::string password);
    void FromCredentials(std::string username, std::string password);
    void FromString(std::string);
    void AddChat(Chat chat);

    std::string ToString() const;
    std::string GetUsername() const;
    std::string GetPassword() const;

};