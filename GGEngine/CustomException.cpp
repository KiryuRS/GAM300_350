#include "stdafx.h"
#include "CustomException.h"

std::string EngineException::make_message(int code, const std::string & source, const std::string & msg)
{
	std::stringstream ss;
	ss << "Exception Data: \n"
		<< "Code    : " << ec.GetString(static_cast<ENG_EXCEPTION>(code)) << "\n"
		<< "Source  : " << source << "\n"
		<< "Message : " << msg << std::endl;

	return ss.str();
}

EngineException::EngineException()
// Delegating Constructor
	: EngineException{ 0, "No Source", "No message" }
{ }

EngineException::EngineException(const std::string & source, const std::string & message)
// Delegating Constructor
	: EngineException{ 0, std::move(source), std::move(message) }
{ }

EngineException::EngineException(const std::string & message)
// Delegating Constructor
	: EngineException{ 0, "No Source", std::move(message) }
{ }

EngineException::EngineException(int code, const std::string & source, const std::string & message)
	: message{ make_message(code, source, message) },
	 ec{ {ASSERTION_ERROR, "Assertion Error"}, {OUT_OF_MEMORY, "Out of Memory"} }
{ }

const char * EngineException::what() const throw()
{
	return message.c_str();
}
