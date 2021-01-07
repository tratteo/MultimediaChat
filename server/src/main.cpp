<<<<<<< HEAD
#pragma once
=======

<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> f09781bac2f3bfa3c5e42b265a1f4962135ee8dd
>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
#include <iostream>
#include <unistd.h>
#include <list>
#include "../include/SSocket.hpp"
#include "../include/ClientHandler.hpp"
<<<<<<< HEAD
=======
<<<<<<< HEAD
=======
=======
#include "../include/SSocket.hpp"
#include "../include/ClientHandler.hpp"
#include <iostream>
#include <unistd.h>
#include <list>
>>>>>>> 857cfa0d7bbe44fd74c36df1f5cee4da5cb0ece9
>>>>>>> f09781bac2f3bfa3c5e42b265a1f4962135ee8dd
>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
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

<<<<<<< HEAD
=======

>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
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

<<<<<<< HEAD
void ReceiveThread()
{

}

=======
>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
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
<<<<<<< HEAD
=======

>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
    }
    return users;

}