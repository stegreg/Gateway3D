#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Display.h"
#include "../Gateway/Graphics.h"
#include "../Gateway/Texture.h"
#include "../Gateway/RenderTarget.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/SystemUI.h"
#include "../Gateway/WindowEventReceiver.h"

#include <list>
#include <map>

struct SDL_Window;
typedef union SDL_Event SDL_Event;

namespace Gateway
{

enum
{
	WINDOW_CENTERED = 1<<0,
	WINDOW_BORDERLESS = 1<<1,
	WINDOW_RESIZABLE = 1<<2,
	WINDOW_HIDDEN = 1<<3,
	WINDOW_FULLSCREEN = 1<<4,
};


class RenderWindow : public RenderTarget
{
	friend class Context;
	friend class Keyboard;
	friend class Mouse;
	friend class SystemUI;

public:
	static Ref<RenderWindow>	Create(const std::string& title, int width, int height, int x = 0, int y = 0, int flags = WINDOW_CENTERED);

public:
	void	Present(bool vsync = false);

	void	SetResizable(bool resizable);

	bool	IsResizable();

	void	SetBordered(bool bordered);
	
	bool	IsBordered();
	
	iVec2	GetSize() const override;

	void	SetFullscreen(const Display::Mode& mode = Display::GetDesktopMode());

	bool	IsFullscreen();

	void	SetWindowed();

	void	SetPosition(int x, int y);

	int		GetPositionX();

	int		GetPositionY();

	void	Hide();

	bool	IsHidden();

	void	Show();

	bool	IsShown();

	void	Minimize();

	void	Maximize();

	void	Restore();

	void	PollEvents();

	bool	IsClosed() const;

	SDL_Window* GetSDLWindow();

	unsigned int GetSDLWindowID() const { return m_id; }

	void AddEventReceiver(unsigned int order, WindowEventFilter* receiver);

	void RemoveEventReceiver(unsigned int order, WindowEventFilter* receiver);

private:
	RenderWindow(const std::string& title, int width, int height, int x, int y, int flags);
	
	~RenderWindow();

private:
	virtual unsigned int GetFBO() const { return 0; }

private:
	static int FilterFunc(void* usrData, SDL_Event* evt);

private:
	unsigned int	m_id = 0;

	bool			m_closed = false;

	Ref<SystemUI>	m_imgui = nullptr;

	std::map<unsigned int, std::list<WindowEventFilter*>> m_receivers;
};


}