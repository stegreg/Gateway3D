#pragma once

#include "../Gateway/RefCounted.h"

typedef union SDL_Event SDL_Event;

namespace Gateway
{

class RenderWindow;

enum
{
	WINDOW_FILTER_ORDER_TOP,
	WINDOW_FILTER_ORDER_UI,
	WINDOW_FILTER_ORDER_WORLD,
};

class WindowEventFilter
{
public:
	WindowEventFilter(unsigned int order, Ref<RenderWindow> window);

	virtual ~WindowEventFilter();

public:
	virtual void OnPreFilter();
	virtual bool OnFilter(SDL_Event* event);
	virtual void OnPostFilter();

	void SetEnabled(bool enabled);
	bool IsEnabled() const;

protected:
	Ref<RenderWindow> m_window;

	unsigned int m_order;

	bool m_enabled = true;
};

}