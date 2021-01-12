#pragma once
#include <iostream>
#include <unistd.h>
#include <string>
#include "Packet.hpp"

int Write(char* buffer, int len, int fd);
int Read(char* buffer, int bufSize, int fd);
bool Send(Packet *packet, int fd);