#include "../Gateway/Window.h"
#include "../Gateway/Graphics.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/UI.h"
#include "../Gateway/SimObject.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <tracy/TracyOpenGL.hpp>

namespace Gateway
{

RenderWindow::RenderWindow(const std::string& title, int width, int height, int x, int y, int flags)
{
	Uint32 sdlFlags = 0;

	if ( flags&WINDOW_BORDERLESS )
		sdlFlags |= SDL_WINDOW_BORDERLESS;
	if ( flags&WINDOW_RESIZABLE )
		sdlFlags |= SDL_WINDOW_RESIZABLE;
	if ( flags&WINDOW_FULLSCREEN )
		sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	if ( ( flags&WINDOW_HIDDEN ) )
		sdlFlags |= SDL_WINDOW_HIDDEN;

	sdlFlags |= SDL_WINDOW_SHOWN;
	sdlFlags |= SDL_WINDOW_OPENGL;

	SDL_Window* sdlWindow = SDL_CreateWindow(
		title.c_str(),
		( flags&WINDOW_CENTERED ) ? SDL_WINDOWPOS_CENTERED : x,
		( flags&WINDOW_CENTERED ) ? SDL_WINDOWPOS_CENTERED : y,
		width, height,
		sdlFlags
	);

	m_id = SDL_GetWindowID(sdlWindow);
}

RenderWindow::~RenderWindow()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_DestroyWindow(sdlWindow);
}

void RenderWindow::AddEventReceiver(unsigned int order, WindowEventFilter* receiver)
{
	m_receivers[order].push_back(receiver);
}

void RenderWindow::RemoveEventReceiver(unsigned int order, WindowEventFilter* receiver)
{
	m_receivers[order].remove(receiver);
}

Ref<RenderWindow> RenderWindow::Create(const std::string& title, int width, int height, int x, int y, int flags)
{
	return Ref<RenderWindow>(new RenderWindow(title, width, height, x, y, flags));
}

void RenderWindow::Present(bool vsync)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_GL_SetSwapInterval(vsync ? 1 : 0);

	SDL_GL_SwapWindow(sdlWindow);

	TracyGpuCollect;
}

void RenderWindow::SetResizable(bool resizable)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_SetWindowResizable(sdlWindow, (SDL_bool)resizable);
}

bool RenderWindow::IsResizable()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	return SDL_GetWindowFlags(sdlWindow)&SDL_WINDOW_RESIZABLE;
}

void RenderWindow::SetBordered(bool bordered)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_SetWindowBordered(sdlWindow, (SDL_bool)bordered);
}

bool RenderWindow::IsBordered()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	return !(SDL_GetWindowFlags(sdlWindow)&SDL_WINDOW_BORDERLESS);
}

iVec2 RenderWindow::GetSize() const
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	int w, h;
	SDL_GetWindowSize(sdlWindow, &w, &h);

	return { w, h };
}

void RenderWindow::SetFullscreen(const Display::Mode& mode)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);
	SDL_DisplayMode sdlMode = {};
	sdlMode.w = mode.width;
	sdlMode.h = mode.height;
	sdlMode.refresh_rate = mode.refreshRate;

	SDL_DisplayMode closestMode = {};
	SDL_GetClosestDisplayMode(0, &sdlMode, &closestMode);

	SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

bool RenderWindow::IsFullscreen()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);
	
	return SDL_GetWindowFlags(sdlWindow)&SDL_WINDOW_FULLSCREEN_DESKTOP;
}

void RenderWindow::SetWindowed()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_SetWindowFullscreen(sdlWindow, 0);
}

void RenderWindow::SetPosition(int x, int y)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_SetWindowPosition(sdlWindow, x, y);
}

int RenderWindow::GetPositionX()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	int x;
	SDL_GetWindowPosition(sdlWindow, &x, NULL);

	return x;
}

int RenderWindow::GetPositionY()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	int y;
	SDL_GetWindowPosition(sdlWindow, NULL, &y);

	return y;
}

void RenderWindow::Hide()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_HideWindow(sdlWindow);
}

bool RenderWindow::IsHidden()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	return SDL_GetWindowFlags(sdlWindow)&SDL_WINDOW_HIDDEN;
}

void RenderWindow::Show()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_ShowWindow(sdlWindow);
}

bool RenderWindow::IsShown()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	return SDL_GetWindowFlags(sdlWindow)&SDL_WINDOW_SHOWN;
}

void RenderWindow::Minimize()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_MinimizeWindow(sdlWindow);
}

void RenderWindow::Maximize()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_MaximizeWindow(sdlWindow);
}

void RenderWindow::Restore()
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_id);

	SDL_RestoreWindow(sdlWindow);
}

int RenderWindow::FilterFunc(void* usrData, SDL_Event* evt)
{
	RenderWindow* wnd = (RenderWindow*)usrData;

	if ( evt->type==SDL_WINDOWEVENT )
	{
		if ( evt->window.windowID!=wnd->m_id )
			return -1;
	}
	if ( evt->type==SDL_KEYDOWN||evt->type==SDL_KEYUP )
	{
		if ( evt->key.windowID!=wnd->m_id )
			return -1;
	}
	if ( evt->type==SDL_MOUSEMOTION )
	{
		if ( evt->motion.windowID!=wnd->m_id )
			return -1;
	}
	if ( evt->type==SDL_MOUSEBUTTONDOWN||evt->type==SDL_MOUSEBUTTONUP )
	{
		if ( evt->button.windowID!=wnd->m_id )
			return -1;
	}
	if ( evt->type==SDL_MOUSEWHEEL )
	{
		if ( evt->wheel.windowID!=wnd->m_id )
			return -1;
	}

	if ( evt->type==SDL_WINDOWEVENT )
	{
		if ( evt->window.event==SDL_WINDOWEVENT_CLOSE )
		{
			wnd->m_closed = true;
		}
	}

	for ( auto receiverGroup:wnd->m_receivers )
	{
		for ( auto receiver:receiverGroup.second )
		{
			if ( receiver->OnFilter(evt) )
				return 0;
		}
	}

	return 0;
}

void RenderWindow::PollEvents()
{
	for ( auto receiverGroup:m_receivers )
	{
		for ( auto receiver:receiverGroup.second )
		{
			receiver->OnPreFilter();
		}
	}

	SDL_PumpEvents(); // TODO: is it safe to put it here ? Alternative: inside Device::Tick()

	SDL_FilterEvents(FilterFunc, this);

	for ( auto receiverGroup:m_receivers )
	{
		for ( auto receiver:receiverGroup.second )
		{
			receiver->OnPostFilter();
		}
	}
}

bool RenderWindow::IsClosed() const
{
	return m_closed;
}

SDL_Window* RenderWindow::GetSDLWindow()
{
	return SDL_GetWindowFromID(m_id);
}

}