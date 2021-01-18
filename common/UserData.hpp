#pragma once
#include <string>
#include <list>
#include <tuple>
#include <algorithm>
#include "Payloads.hpp"
#include "ChatData.hpp"

class UserData
{

    private: 
    std::string username;
    std::string password;
    std::list<ChatData*> chats;

    public:
    ~UserData();
    UserData();
    UserData(std::string username, std::string password);
    void FromCredentials(std::string username, std::string password);
    void FromString(std::string);
    void AddChat(ChatData* chat);

    std::string ToString() const;
    ChatData* GetChatWith(std::string username);
    inline std::list<ChatData*> GetChats() const { return chats; };
    inline std::string GetUsername() const { return username; };
    inline std::string GetPassword() const { return password; };

};