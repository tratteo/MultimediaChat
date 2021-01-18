CC			= g++
CFLAGS 		= -pthread -std=c++11
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
CMMSRCDIR 	= ../common/src

SRCS 		= 	$(SRCDIR)/server.cpp $(SRCDIR)/ClientHandler.cpp  \
			  	$(CMMSRCDIR)/ClientSessionData.cpp $(CMMSRCDIR)/errhandler.cpp $(SRCDIR)/Serializer.cpp \
			  	$(CMMSRCDIR)/UserData.cpp $(CMMSRCDIR)/Payloads.cpp $(CMMSRCDIR)/Packet.cpp \
			  	$(CMMSRCDIR)/NetworkHandler.cpp $(SRCDIR)/DatabaseHandler.cpp $(CMMSRCDIR)/ChatData.cpp \
			  	$(CMMSRCDIR)/ByteBufferUtils.cpp $(CMMSRCDIR)/UDPSocket.cpp $(CMMSRCDIR)/CSocket.cpp

OBJS 		= $(SRCS:.c=.o)
TARGET		= mc_server.out

.PHONY: depend clean

all: $(TARGET)
	@echo  Multimedia ChatData server compiled, run with ./$(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	makedepend $(INCLUDES) $^