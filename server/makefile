CC			= g++
CFLAGS 		= -pthread -std=c++11
INCLUDES 	= -I/include -I../common
SRCDIR 		= src
SRCS 		= $(SRCDIR)/main.cpp $(SRCDIR)/ClientHandler.cpp $(SRCDIR)/SSocket.cpp $(SRCDIR)/ClientSessionData.cpp ../common/src/errhandler.cpp 
OBJS 		= $(SRCS:.c=.o)
TARGET		= mc_server

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