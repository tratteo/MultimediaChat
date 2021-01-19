#include "../Poller.hpp"

std::string PollCinOnce(pollfd info[], int size, int index, int delay,std::function<bool()> condition)
{
    std::string line;
    bool polled = false;
    while(!condition() && !polled)
    {
        poll(info, size, delay);
        if(info[index].revents & POLLIN)
		{
            std::getline(std::cin, line);
			polled = true;
		}
    }
    return line;
}

void PollFdLoop(pollfd info[], int size, int index, int delay, std::function<bool()> condition, std::function<void(char* buf, int read)> body)
{
    char buf[1024];
    while(!condition())
    {
        poll(info, size, delay);
        if(info[index].revents & POLLIN)
		{
            int bytesRead = read(info[index].fd, buf, 1024);
            body(buf, bytesRead);
            memset(&buf, 0, 1024);
        }
    }
    return;
}

void PollFdLoop(pollfd info[], int size, int index, int delay, int bufSize, std::function<bool()> condition, std::function<void(char* buf, int read)> body)
{
    char buf[bufSize];
    while(!condition())
    {
        poll(info, size, delay);
        if(info[index].revents & POLLIN)
		{
            int bytesRead = read(info[index].fd, buf, bufSize);
            body(buf, bytesRead);
            memset(&buf, 0, bufSize);
        }
        else
        {
            //std::cout<<"Polling";
            std::flush(std::cout);
        }

    }
    return;     
}
