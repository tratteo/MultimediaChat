#pragma once
#include <iostream>
#include <unistd.h>
#include <string>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include "Packet.hpp"

int Write(char* buffer, int len, int fd);
int Read(char* buffer, int bufSize, int fd);
bool Send(Packet *packet, int fd);