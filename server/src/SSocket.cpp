#include "../include/SSocket.hpp"

SSocket::SSocket()
{
    
}

SSocket::~SSocket()
{
    if(close(server_fd) < 0)
    {
        handle_fatal_error("Unable to close server socket fd");
    }
}

void SSocket::Init(int type, int protocol)
{
    if ((server_fd = socket(AF_INET, type, protocol)) == 0) 
    { 
        handle_error("Unable to open server socket");
    } 
       
    // Forcefully attaching socket to the port 8080 
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        handle_fatal_error("Unable to setsockopt");
    } 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    addrlen = sizeof(address);

    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    { 
        handle_fatal_error("Unable to bind the server socket");
    } 

    if (listen(server_fd, 3) < 0) 
    { 
        handle_fatal_error("Unable to listen to the server socket");
    }
}

ClientSessionData* SSocket::AcceptConnection()
{
    int socket_fd;
    if ((socket_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
    { 
        handle_error("Unable to connect");
        return nullptr;
    }
    else
    {
        char* ip = inet_ntoa(address.sin_addr);
        ClientSessionData *data = new ClientSessionData(socket_fd, ip);
        return data;
    }
     
}