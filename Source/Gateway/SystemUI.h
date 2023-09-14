#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/WindowEventReceiver.h"
#include "../Gateway/Context.h"

#include <imgui.h>

typedef union SDL_Event SDL_Event;

namespace Gateway
{

class SystemUI : public RefCounted, public WindowEventFilter
{
	friend class RenderWindow;

public:
	static Ref<SystemUI> Create(Ref<RenderWindow> window, Ref<Context> context);

public:
	void Begin();

	void End();

private:
	SystemUI(Ref<RenderWindow> window, Ref<Context> context);

	~SystemUI();

private:
	bool OnFilter(SDL_Event* event) override;
};

}