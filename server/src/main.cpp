
#include "../include/SSocket.hpp"
#include "../include/ClientHandler.hpp"
#include <iostream>
#include <unistd.h>
#include <list>
#include "../include/Serializer.hpp"
#include "../include/UserData.hpp"

void CloseService(int);
std::list<UserData*> PopulateUsers();

int main()
{
    SSocket *serverSocket = new SSocket();
    std::cout<<"Initializing server..."<<std::endl;
    serverSocket->init(SOCK_STREAM, 0);

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);


    auto users = PopulateUsers();

    while(true)
    {
        ClientSessionData *data; 
        if((data=serverSocket->AcceptConnection()) != nullptr)
        {
            std::cout<<"Accepted"<<std::endl;
            ClientHandler *handler = new ClientHandler(data, users);
            handler->HandleConnection();
        }
    }
}

void CloseService(int signal)
{
    exit(EXIT_SUCCESS);
}

std::list<UserData*> PopulateUsers()
{
    std::list<UserData*> users;
    Serializer *ser = new Serializer("usersData.txt");
    auto lines = ser->GetLines();
    if(lines.size() > 0)
    {
        for (auto const& i : lines) 
        {
            int index = i.find("-");
            std::string user = i.substr(0, index);
            std::string password = i.substr(index+1, i.length());
            UserData *data = new UserData(user, password);
            std::cout << "Adding: "+data->ToString();
            users.push_front(data);
        }

    }
    return users;

}