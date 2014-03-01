
#ifndef GENERATOR_H
#define GENERATOR_H

#include <exception>

class GenException : public std::exception
{
	std::string msg;

public:
	
	GenException(const std::string &msg) :
		msg(msg)
	{}

	~GenException() throw() {}
	
	const char *what() const throw()
	{
		return msg.c_str();
	}
};

#endif
