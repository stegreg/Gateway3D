#include "../Gateway/System.h"

#include <SDL2/SDL.h>

namespace Gateway
{

void System::Sleep(int milliseconds)
{
	SDL_Delay(milliseconds);
}

unsigned long long System::GetTime()
{
	return SDL_GetTicks64();
}

}
