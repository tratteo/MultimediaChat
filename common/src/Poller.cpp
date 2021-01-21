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

void PollFdLoop(pollfd info[], int size, int index, int delay, int bufSize, std::function<bool()> condition, std::function<void(char* buf, int read, int recycle)> body)
{
    int recycle = 0;
    char buf[bufSize];
    while(!condition())
    {
        recycle++;
        int res = poll(info, size, delay);
        if(res > 0 && info[index].revents & POLLIN)
		{
            recycle = 0;
            int bytesRead = read(info[index].fd, buf, bufSize);
            body(buf, bytesRead, recycle);
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
