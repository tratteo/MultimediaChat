CC			= g++
CFLAGS 		= -pthread -std=c++11
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
SRCS 		= $(SRCDIR)/server.cpp $(SRCDIR)/ClientHandler.cpp $(SRCDIR)/SSocket.cpp \
			  $(SRCDIR)/ClientSessionData.cpp ../common/src/errhandler.cpp $(SRCDIR)/Serializer.cpp \
			  $(SRCDIR)/UserData.cpp ../common/src/Payloads.cpp ../common/src/Packet.cpp ../common/src/NetworkHandler.cpp $(SRCDIR)/DatabaseHandler.cpp $(SRCDIR)/ChatData.cpp
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