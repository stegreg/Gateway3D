#include "../Gateway/WindowEventReceiver.h"
#include "../Gateway/Window.h"

namespace Gateway
{

WindowEventFilter::WindowEventFilter(unsigned int order, Ref<RenderWindow> window)
	: m_window(window), m_order(order)
{
	m_window->AddEventReceiver(m_order, this);
}

WindowEventFilter::~WindowEventFilter()
{
	m_window->RemoveEventReceiver(m_order, this);
}

void WindowEventFilter::OnPreFilter()
{
}

bool WindowEventFilter::OnFilter(SDL_Event* event)
{
	return false;
}

void WindowEventFilter::OnPostFilter()
{
}

void WindowEventFilter::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

bool WindowEventFilter::IsEnabled() const
{
	return m_enabled;
}

}