all: src/CSocket.cpp src/main.cpp src/errhandler.cpp 
	g++ -o MultimediaChat include/CSocket.hpp include/errhandler.hpp src/CSocket.cpp src/main.cpp src/errhandler.cpp

clean: 
	-rm MultimediaChat