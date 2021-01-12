#pragma once
#include <string>
#include <tuple>
#include <list>
#include "../../common/Payloads.hpp"

class ChatData
{
    public:
    ChatData();
    ChatData(std::string firstUser, std::string secondUser);
    std::tuple<std::string, std::string> GetUsers();
    void AddMessage(MessagePayload message);
    std::string ToString(std::string owner);

    private:
    std::string secondUser;
    std::string firstUser;
    std::list<MessagePayload> messages;

};