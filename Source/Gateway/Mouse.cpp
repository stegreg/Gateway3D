#include "../Gateway/Mouse.h"
#include "../Gateway/Window.h"

#include <SDL2/SDL.h>

namespace Gateway
{

Ref<Mouse> Mouse::Create(Ref<RenderWindow> window)
{
	return Ref<Mouse>(new Mouse(window));
}

void Mouse::SetRelativeMode(bool enabled)
{
	SDL_SetRelativeMouseMode(enabled ? SDL_TRUE: SDL_FALSE);
}

bool Mouse::IsButtonDown(unsigned int button) const
{
	return m_currentState.at(button)==DOWN;
}

bool Mouse::IsButtonUp(unsigned int button) const
{
	return m_currentState.at(button)==UP;
}

bool Mouse::IsButtonReleased(unsigned int button) const
{
	return m_currentState.at(button)==RELEASED;
}

bool Mouse::IsButtonPressed(unsigned int button) const
{
	return m_currentState.at(button)==PRESSED;
}

unsigned int Mouse::GetButtonState(unsigned int button) const
{
	return m_currentState.at(button);
}

iVec2 Mouse::GetRelativeMotion() const
{
	return m_relativeMotion;
}

iVec2 Mouse::GetLocalPosition() const
{
	return m_localPosition;
}

void Mouse::WrapInWindow(bool wrap)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_window->m_id);

	SDL_SetWindowGrab(sdlWindow, wrap ? SDL_TRUE : SDL_FALSE);
}

void Mouse::SetLocalPosition(const iVec2& localPosition)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_window->m_id);

	SDL_WarpMouseInWindow(sdlWindow, localPosition.x, localPosition.y);
}

void Mouse::CenterLocalPosition()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_window->m_id);

	int w, h;
	SDL_GetWindowSize(sdlWindow, &w, &h);

	SDL_WarpMouseInWindow(sdlWindow, w/2, h/2);
}

void Mouse::SetVisible(bool visible)
{
	SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void Mouse::Flush()
{
	m_relativeMotion.x = 0;
	m_relativeMotion.y = 0;

	for ( unsigned int button = 0; button<m_currentState.size(); button++ )
	{
		m_currentState.at(button) = m_lastState.at(button) = UP;
	}
}

Mouse::Mouse(Ref<RenderWindow> window)
	: WindowEventFilter(WINDOW_FILTER_ORDER_WORLD, window)
{
	m_currentState.fill(UP);
	m_lastState.fill(UP);
}

Mouse::~Mouse()
{
}

void Mouse::OnPreFilter()
{
	m_relativeMotion.x = 0;
	m_relativeMotion.y = 0;
}

bool Mouse::OnFilter(SDL_Event* evt)
{
	if ( !m_enabled )
		return false;

	if ( evt->type==SDL_MOUSEBUTTONDOWN||evt->type==SDL_MOUSEBUTTONUP )
	{
		if ( evt->button.windowID==m_window->GetSDLWindowID() )
		{
			unsigned int button = ConvertButton(evt->button.button);

			m_currentState.at(button) = ( evt->button.type==SDL_MOUSEBUTTONDOWN ? PRESSED : RELEASED );
			
			return true;
		}
	}

	if ( evt->type==SDL_MOUSEMOTION )
	{
		if ( evt->motion.windowID==m_window->GetSDLWindowID() )
		{
			m_relativeMotion.x += evt->motion.xrel;
			m_relativeMotion.y += evt->motion.yrel;

			m_localPosition.x = evt->motion.x;
			m_localPosition.y = evt->motion.y;

			return true;
		}
	}

	return false;
}

void Mouse::OnPostFilter()
{
	for ( unsigned int button = 0; button<m_currentState.size(); button++ )
	{
		if ( m_lastState.at(button)==PRESSED&&m_currentState.at(button)==PRESSED )
			m_currentState.at(button) = DOWN;
		else if ( m_lastState.at(button)==RELEASED&&m_currentState.at(button)==RELEASED )
			m_currentState.at(button) = UP;

		m_lastState.at(button) = m_currentState.at(button);
	}
}

unsigned int Mouse::ConvertButton(unsigned int sdlButton)
{
	switch ( sdlButton )
	{
	case SDL_BUTTON_LEFT:
		return MOUSE_LEFT;
	case SDL_BUTTON_RIGHT:
		return MOUSE_RIGHT;
	case SDL_BUTTON_MIDDLE:
		return MOUSE_MIDDLE;
	case SDL_BUTTON_X1:
		return MOUSE_X1;
	case SDL_BUTTON_X2:
		return MOUSE_X2;
	}

	return 0;
}

}
