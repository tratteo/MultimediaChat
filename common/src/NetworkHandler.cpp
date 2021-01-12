#include "NetworkHandler.hpp"

int Write(char* buffer, int len, int fd)
{
	int written = 0;
	while (written < len)
	{
		written += write(fd, buffer + written, len - written);
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