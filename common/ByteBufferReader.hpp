#include <iostream>
#include <unistd.h>
#include <string>

int Write(char* buffer, int len, int fd);
int Read(char* buffer, int bufSize, int fd);