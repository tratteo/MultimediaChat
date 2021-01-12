#pragma once
#include <string>
#include <list>
#include <algorithm>
#include "../../common/Payloads.hpp"
#include "../include/ChatData.hpp"

class UserData
{

    private: 
    std::string username;
    std::string password;
    std::list<ChatData*> chats;

    public:
    UserData();
    UserData(std::string username, std::string password);
    void FromCredentials(std::string username, std::string password);
    void FromString(std::string);
    void AddChat(ChatData* chat);

    std::string ToString() const;
    ChatData* GetChatWith(std::string username);
    std::list<ChatData*> GetChats() const;
    std::string GetUsername() const;
    std::string GetPassword() const;

};