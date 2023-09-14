#include "../Gateway/Context.h"
#include "../Gateway/Window.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <tracy/TracyOpenGL.hpp>

namespace Gateway
{
unsigned int Context::s_currentID = 0;

Ref<Context> Context::Create(Ref<RenderWindow> window)
{
	return Ref<Context>(new Context(window));
}

void Context::MakeCurrent(Ref<Context> context)
{
	SDL_Window* sdlWindow = SDL_GetWindowFromID(context->GetWindow()->m_id);

	SDL_GL_MakeCurrent(sdlWindow, context->m_sdlContext);

	s_currentID = context->m_id;
}

unsigned int Context::GetCurrentID()
{
	return s_currentID;
}

Context::Context(Ref<RenderWindow> window)
	: m_window(window)
{
	static unsigned int lastID = 0;

	m_id = ++lastID;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* sdlWindow = SDL_GetWindowFromID(m_window->m_id);

	m_sdlContext = SDL_GL_CreateContext(sdlWindow);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	s_currentID = m_id;

	TracyGpuContext;
}

Context::~Context()
{
	SDL_GL_DeleteContext(m_sdlContext);
}

Ref<RenderWindow> Context::GetWindow()
{
	return m_window;
}

}