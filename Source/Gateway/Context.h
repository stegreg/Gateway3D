#pragma once

#include "../Gateway/RefCounted.h"

namespace Gateway
{

typedef void* SDL_GLContext;

class RenderWindow;

class Context : public RefCounted
{
public:
	static Ref<Context> Create(Ref<RenderWindow> window);

	static void			MakeCurrent(Ref<Context> context);

	static unsigned int	GetCurrentID();

public:
	Ref<RenderWindow> GetWindow();

	unsigned int GetID() const { return m_id; }

	SDL_GLContext GetContextSDL() { return m_sdlContext; }

private:
	Context(Ref<RenderWindow> window);

	~Context();

private:
	Ref<RenderWindow>  m_window;
	
	unsigned int m_id;

	void*		 m_sdlContext;

	static unsigned int s_currentID;
};

}