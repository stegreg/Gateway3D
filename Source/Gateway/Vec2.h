#pragma once

namespace Gateway
{

template<class T>
class Vec2
{
public:
	union
	{
		struct
		{
			T x, y;		
		};

		struct
		{
			T r, g;
		};

		struct
		{
			T u, v;
		};
	};

	Vec2& operator=(const Vec2& other)
	{
		x = other.x;
		y = other.y;

		return *this;
	}


	Vec2& operator+=(const Vec2& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	Vec2& operator-=(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	Vec2& operator*=(const Vec2& other)
	{
		x *= other.x;
		y *= other.y;

		return *this;
	}

	Vec2& operator/=(const Vec2& other)
	{
		x /= other.x;
		y /= other.y;

		return *this;
	}

	Vec2& operator/=(const float& scalar)
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}

	const Vec2 operator/(const float& scalar) const
	{
		return { x/scalar, y/scalar };
	}
};

using fVec2 = Vec2<float>;
using iVec2 = Vec2<int>;
using uVec2 = Vec2<unsigned int>;

}