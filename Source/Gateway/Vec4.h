#pragma once
#include "../Gateway/Vec3.h"

namespace Gateway
{

struct Vec4
{
	union
	{
		struct
		{
			float x, y, z, w;
		};

		struct
		{
			float r, g, b, a;
		};

		struct
		{
			float x, y, width, height;
		};
	};

public:
	Vec3 XYZ() const;

};

}