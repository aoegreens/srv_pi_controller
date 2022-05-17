#pragma once

#include "common/Externals.h"

struct RequiredParameter
{
	std::string upstreamKey;
	std::string name;
	std::vector<std::string> values;
	std::string defaultVal;
};
