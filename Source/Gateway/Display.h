#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace Gateway
{

class Display
{
public:
	struct Mode
	{
		int width = 0, height = 0;
		int refreshRate = 0;
	};

	static std::vector<Mode>	GetModes();

	static Mode					GetDesktopMode();
};

}