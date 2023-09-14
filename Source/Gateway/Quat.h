#pragma once

#include "../Gateway/Vec3.h"

namespace Gateway
{

struct Quat
{
	float x, y, z, w;

	Quat  operator-();

	Quat& operator=(const Quat& other);
	
	Quat& operator*=(const Quat& other);

	Quat  operator*(const Quat& other) const;

	Vec3  operator*(const Vec3& vec) const;

	float Length() const;

	float LengthSquared() const;

	Quat  Inversed() const;

	Quat& Normalize();

	Quat& FromIdentity();

	Quat& FromRotation(float angle, const Vec3& globalAxis);
};

}