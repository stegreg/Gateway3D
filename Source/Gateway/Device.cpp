#include "../Gateway/Device.h"

#include "../Gateway/SimObject.h"

#include <SDL2/SDL.h>

namespace Gateway
{
Device* Device::s_singleton = nullptr;

Ref<Device> Device::Create()
{
	if ( !s_singleton )
	{
		s_singleton = new Device;

		return s_singleton;
	}

	return nullptr;
}

Ref<Device> Device::Get()
{
	return s_singleton;
}

Device::Device()
{
	SimObjectRegistrar::RegisterAllTypes();

	SDL_Init(SDL_INIT_EVENTS);
}

Device::~Device()
{
	s_singleton = nullptr;

	SDL_Quit();
}

}