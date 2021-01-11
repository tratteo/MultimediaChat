
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
    std::string firstUser;
    std::string secondUser;
    std::list<MessagePayload> messages;

    void AddMessage(MessagePayload message)
    {
        messages.push_back(message);
    }

    std::string ToString()
    {
        std::string res = firstUser + "-" + secondUser + "\n";
        for (auto& msg : messages)
        {
            res.append(msg.ToString());
        }
        return res;
    }
};