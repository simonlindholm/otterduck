#pragma once
#include "cl.h"
#include <vector>

// This is the Program class that is called by CL_ClanApplication
class Program {
	public:
		static int main(const std::vector<CL_String>& args);
};

