#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/InputState.h"
#include "../Gateway/WindowEventReceiver.h"

#include <array>

typedef union SDL_Event SDL_Event;

namespace Gateway
{

enum : unsigned
{
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_F16,
	KEY_F17,
	KEY_F18,
	KEY_F19,
	KEY_F20,
	KEY_LCTRL,
	KEY_LSHIFT,
	KEY_LALT,
	KEY_LGUI,
	KEY_RCTRL,
	KEY_RSHIFT,
	KEY_RALT,
	KEY_RGUI,
	KEY_RETURN,
	KEY_ESCAPE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_SPACE,
	KEY_HOME,
	KEY_PAGEUP,
	KEY_DELETE,
	KEY_END,
	KEY_PAGEDOWN,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_DOWN,
	KEY_UP,
	KEY_MINUS,
	KEY_EQUALS,
	KEY_LEFTBRACKET,
	KEY_RIGHTBRACKET,
	KEY_BACKSLASH,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_GRAVE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_CAPSLOCK,
	KEY_PRINTSCREEN,
	KEY_SCROLLLOCK,
	KEY_PAUSE,
	KEY_INSERT,
	KEY_NUMLOCKCLEAR,
	KEY_KP_DIVIDE,
	KEY_KP_MULTIPLY,
	KEY_KP_MINUS,
	KEY_KP_PLUS,
	KEY_KP_ENTER,
	KEY_KP_1,
	KEY_KP_2,
	KEY_KP_3,
	KEY_KP_4,
	KEY_KP_5,
	KEY_KP_6,
	KEY_KP_7,
	KEY_KP_8,
	KEY_KP_9,
	KEY_KP_0,
	KEY_KP_PERIOD,
	KEY_KP_EQUALS,
};

class Keyboard : public RefCounted, public WindowEventFilter
{
	friend class RenderWindow;

public:
	static Ref<Keyboard> Create(Ref<RenderWindow> window);

public:
	bool		 IsKeyDown(unsigned int key) const;

	bool		 IsKeyPressed(unsigned int key) const;

	bool		 IsKeyReleased(unsigned int key) const;

	bool		 IsKeyUp(unsigned int key) const;

	unsigned int GetKeyState(unsigned int key) const;

	void		 Flush();

private:
	Keyboard(Ref<RenderWindow> window);

private:
	unsigned int ConvertKey(unsigned int key);

	void		 OnPreFilter() override;
	bool		 OnFilter(SDL_Event* event) override;
	void		 OnPostFilter() override;

private:
	std::array<unsigned int, 112> m_currentState, m_lastState;

};

}