CC			= g++
CFLAGS 		= -pthread -std=c++11
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
SRCS 		= $(SRCDIR)/main.cpp $(SRCDIR)/ClientHandler.cpp $(SRCDIR)/SSocket.cpp \
			  $(SRCDIR)/ClientSessionData.cpp ../common/src/errhandler.cpp $(SRCDIR)/Serializer.cpp \
<<<<<<< HEAD
			  $(SRCDIR)/UserData.cpp ../common/packets/MessagePacket.cpp ../common/packets/CredentialsPacket.cpp ../common/src/ByteBufferReader.cpp
=======
			  $(SRCDIR)/UserData.cpp
>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
OBJS 		= $(SRCS:.c=.o)
TARGET		= mc_server.out

.PHONY: depend clean

all: $(TARGET)
	@echo  Multimedia Chat server compiled, run with ./$(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	makedepend $(INCLUDES) $^