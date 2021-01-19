CC			= g++
CFLAGS 		= -std=c++11 -pthread
LFLAGS		= -lasound
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
CMMSRCDIR 	= ../common/src

SRCS 		= 	$(SRCDIR)/client.cpp $(SRCDIR)/Client.cpp $(CMMSRCDIR)/CSocket.cpp $(CMMSRCDIR)/errhandler.cpp $(CMMSRCDIR)/Payloads.cpp \
			  	$(CMMSRCDIR)/NetworkHandler.cpp $(CMMSRCDIR)/Packet.cpp $(CMMSRCDIR)/SoundRegistrer.cpp $(CMMSRCDIR)/UDPSocket.cpp \
				$(CMMSRCDIR)/ByteBufferUtils.cpp $(CMMSRCDIR)/ClientSessionData.cpp $(CMMSRCDIR)/SoundPlayer.cpp $(CMMSRCDIR)/UserData.cpp \
				$(CMMSRCDIR)/Poller.cpp

OBJS 		= $(SRCS:.c=.o)
TARGET		= mc_client.out

.PHONY: depend clean

all: $(TARGET)
	@echo  Multimedia ChatData client compiled, run with ./$(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	makedepend $(INCLUDES) $^