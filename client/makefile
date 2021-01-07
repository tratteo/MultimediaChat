CC			= g++
<<<<<<< HEAD
CFLAGS 		= -std=c++11 -pthread
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
SRCS 		= $(SRCDIR)/main.cpp $(SRCDIR)/CSocket.cpp ../common/src/errhandler.cpp ../common/packets/CredentialsPacket.cpp ../common/src/ByteBufferReader.cpp
=======
CFLAGS 		= -std=c++11
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
SRCS 		= $(SRCDIR)/main.cpp $(SRCDIR)/CSocket.cpp ../common/src/errhandler.cpp
>>>>>>> 64672db9cb0205e382ca6be2ac8c5354714d018f
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