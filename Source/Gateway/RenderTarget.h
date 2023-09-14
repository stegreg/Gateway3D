#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Vec2.h"

namespace Gateway
{

class RenderTarget : public RefCounted
{
public:
	virtual iVec2		 GetSize() const = 0;

	virtual unsigned int GetFBO() const = 0;
};

}