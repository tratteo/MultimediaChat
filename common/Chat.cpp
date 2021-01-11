
#pragma once
#include <string>
#include <list>
#include "Payloads.cpp"

class Chat
{
    public:

    Chat()
    {

    }

    Chat(std::string firstUser, std::string secondUser)
    {
        this->firstUser = firstUser;
        this->secondUser = secondUser;
    }

    std::string secondUser;
    std::string firstUser;

    std::list<MessagePayload> messages;

    void AddMessage(MessagePayload message)
    {
        messages.push_back(message);
    }

    std::string ToString(std::string owner)
    {
        std::string subject = owner == firstUser ? secondUser : firstUser;
        std::string res = "Chat:" + subject + "\n";
        for (auto& msg : messages)
        {
            res.append(msg.ToString());
        }
        return res;
    }
};