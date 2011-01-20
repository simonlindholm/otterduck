#pragma once
#include "cl.h"
#include <string>

class ConfigException : public CL_Exception {
	public:
		ConfigException(const std::string& msg) : CL_Exception(msg) {}
};
