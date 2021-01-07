#include <iostream>
#include <unistd.h>
#include <string.h>

int Write(char* buffer, int len, int fd);
int Read(char* buffer, int bufSize, int fd);