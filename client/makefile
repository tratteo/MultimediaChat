CC			= g++
CFLAGS 		= -std=c++11 -pthread
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
SRCS 		= $(SRCDIR)/main.cpp $(SRCDIR)/CSocket.cpp ../common/src/errhandler.cpp ../common/packets/CredentialsPacket.cpp ../common/src/ByteBufferReader.cpp
OBJS 		= $(SRCS:.c=.o)
TARGET		= mc_client.out

.PHONY: depend clean

all: $(TARGET)
	@echo  Multimedia Chat client compiled, run with ./$(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	makedepend $(INCLUDES) $^