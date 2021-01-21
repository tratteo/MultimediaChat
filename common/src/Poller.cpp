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

void PollFdLoop(pollfd info[], int size, int index, int delay, std::function<bool()> condition, std::function<void(bool pollin, int recycle)> body)
{
    int recycle = 0;
    while(!condition())
    {
        std::flush(std::cout);
        recycle++;
        int res = poll(info, size, delay);
        bool pollin = res > 0 && info[index].revents & POLLIN;
        if(pollin)
        {
            recycle = 0x0;
        }
        body(pollin, recycle);
        /* if(res > 0 && info[index].revents & POLLIN)
		{
            recycle = 0;
            int bytesRead = read(info[index].fd, buf, bufSize);
            body(buf, bytesRead, recycle);
            memset(&buf, 0, bufSize);
        }
        else
        {
            std::flush(std::cout);
        }
        std::cout<<recycle<<std::endl;
        std::flush(std::cout); */
    }
    return;     
}
