#pragma once
#include <string>
#include <list>
#include "../../common/Payloads.hpp"

class ChatData
{
    public:

    ChatData();
    ChatData(std::string firstUser, std::string secondUser);

    std::string secondUser;
    std::string firstUser;
    std::list<MessagePayload> messages;

    void AddMessage(MessagePayload message);
    std::string ToString(std::string owner);
};