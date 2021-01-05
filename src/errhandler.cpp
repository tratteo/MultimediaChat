#include "../include/errhandler.hpp"

void handle_error(std::string message)
{
	std::cerr << message << "Error: " << errno << std::endl;
}

void handle_fatal_error(std::string message)
{
	handle_error(message);
	exit(EXIT_FAILURE);
}