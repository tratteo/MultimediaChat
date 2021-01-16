#include "../NetworkHandler.hpp"

int Write(char* buffer, int len, int fd)
{
	int written = 0;
	while (written < len)
	{
		int res = write(fd, buffer + written, len - written);
		if(res != -1)
		{
			written += res;
		}
		else
		{
			return -1;
		}
		
	}
	return written;
}

int Read(char* buffer, int bufSize, int fd)
{
	int bytesRead = read(fd, buffer, bufSize);
	if (bytesRead == -1)
	{
		return -1;
	}
	return bytesRead;
}

bool Send(Packet *packet, int fd)
{
	int packetLen = packet->GetTotalLength();
	int sent = Write(packet->Serialize(), packetLen, fd);
	return sent == packetLen;
}

int WriteTo(char* buffer, int len, int fd, struct sockaddr_in* addr)
{
	int written = 0;
	while (written < len)
	{
		sockaddr_in addrv = *addr;
		int res = sendto(fd, buffer, len, 0, (struct sockaddr*)addr, sizeof(addrv));
		if (res != -1)
		{
			written += res;
		}
		else
		{
			return -1;
		}

	}
	return written;
}