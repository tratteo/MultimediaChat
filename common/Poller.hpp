#pragma once
#include <poll.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <functional>

std::string PollCinOnce(pollfd info[], int size, int index, int delay, std::function<bool()> condition);
void PollFdLoop(pollfd info[], int size, int index, int delay, std::function<bool()> condition, std::function<void(char* buf, int read)> body);
void PollFdLoop(pollfd info[], int size, int index, int delay, int bufSize, std::function<bool()> condition, std::function<void(char* buf, int read)> body);