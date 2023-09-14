#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/InputState.h"
#include "../Gateway/Vec2.h"
#include "../Gateway/WindowEventReceiver.h"

#include <array>

typedef union SDL_Event SDL_Event;

namespace Gateway
{

class RenderWindow;

enum
{
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT,
	MOUSE_X1,
	MOUSE_X2,
};

class Mouse : public RefCounted, public WindowEventFilter
{
	friend class RenderWindow;

public:
	static Ref<Mouse> Create(Ref<RenderWindow> window);
	
	static void		  SetRelativeMode(bool enabled);

public:
	bool		 IsButtonDown(unsigned int button) const;

	bool		 IsButtonUp(unsigned int button) const;

	bool		 IsButtonReleased(unsigned int button) const;

	bool		 IsButtonPressed(unsigned int button) const;

	unsigned int GetButtonState(unsigned int button) const;

	iVec2		 GetRelativeMotion() const;

	iVec2		 GetLocalPosition() const;

	void		 WrapInWindow(bool wrap);

	void		 SetLocalPosition(const iVec2& localPosition);

	void		 CenterLocalPosition();

	void		 SetVisible(bool visible);

	void		 Flush();

private:
	Mouse(Ref<RenderWindow> window);
	
	~Mouse();

private:
	void		 OnPreFilter() override;
	bool		 OnFilter(SDL_Event* event) override;
	void		 OnPostFilter() override;

	unsigned int ConvertButton(unsigned int sdlButton);

private:
	std::array<unsigned int, 5> m_currentState, m_lastState;

	iVec2 m_localPosition = { 0, 0 };

	iVec2 m_relativeMotion = { 0, 0 };
};

}