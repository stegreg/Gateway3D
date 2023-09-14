#pragma once

#include "../Gateway/Model.h"

#include <vector>

namespace Gateway
{

class Box : public Model
{
public:
	Box(float width = 1.0f, float height = 1.0f, float depth = 1.0f);

	void SetWidth(float width);

	void SetHeight(float height);
	
	void SetDepth(float depth);
};

}