#include "../Gateway/Display.h"

#include <SDL2/SDL.h>

namespace Gateway
{

std::vector<Display::Mode> Display::GetModes()
{
	std::vector<Display::Mode> displayModes;

	int numDisplayModes = SDL_GetNumDisplayModes(0);

	for ( int i = 0; i<numDisplayModes; i++ )
	{
		SDL_DisplayMode mode;
		SDL_GetDisplayMode(0, i, &mode);

		displayModes.push_back({ mode.w, mode.h, mode.refresh_rate });
	}

	return displayModes;
}

Display::Mode Display::GetDesktopMode()
{
	SDL_DisplayMode sdlDisplayMode = {};
	SDL_GetDesktopDisplayMode(0, &sdlDisplayMode);

	return { sdlDisplayMode.w, sdlDisplayMode.h, sdlDisplayMode.refresh_rate };
}

}