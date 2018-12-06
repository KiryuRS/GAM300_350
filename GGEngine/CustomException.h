#pragma once
#include <exception>
#include "EnumParser.h"

/*
The custom exception will be used in cases of any messages that we would wanna throw:
Reasons for using could be of the following:
- Assertion system (from our TOOLS)
- Memory Manager (if we are using)
- Custom data structure exceptions
- Core System exceptions

References:
https://stackoverflow.com/questions/34420388/c-custom-exception-derived-from-stdexception-not-being-caught
*/

// Custom Exception codes will go in here. Determine what kind of exceptions to contain
enum ENG_EXCEPTION : int
{
	ASSERTION_ERROR = 0,
	OUT_OF_MEMORY,
};

class EngineException : public std::exception
{
	EnumConversions<ENG_EXCEPTION> ec;
	std::string message;
	// helper function
	std::string make_message(int code, const std::string& source, const std::string& message);

public:
	EngineException();
	EngineException(const std::string& source, const std::string& message);
	explicit EngineException(const std::string& message);
	EngineException(int code, const std::string& source, const std::string& message);
	virtual const char* what() const throw();
};