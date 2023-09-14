#pragma once

namespace Gateway
{

class System
{
public:
	static void Sleep(int milliseconds);

	static unsigned long long GetTime();
};

}