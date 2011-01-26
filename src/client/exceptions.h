#pragma once
#include <string>

class ConfigException : public std::exception {
	private:
		std::string str;
	public:
		ConfigException(const std::string& msg) : str(msg) {}
		virtual ~ConfigException() throw() {}
		virtual const char* what() const throw() { return str.c_str(); }
};
