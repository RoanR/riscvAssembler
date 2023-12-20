#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <stdexcept>

int preprocessing(std::string src, std::string dest);

/*
 * A Custom Exception Class to be thrown when a line isn't properly formatted
 * @param msg Constructor takes string which can be accessed through what()
 * @public what() returns msg inputted when constructor was called
 */
class bad_line: public std::exception 
{
	private:
	std::string msg;

	public:
	// Constructor 
	bad_line(std::string msg) : msg(msg) {};

	// To Print the error
	std::string what()
	{
		return msg;
	}
};

#endif