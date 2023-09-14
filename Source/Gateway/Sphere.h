#pragma once

#include "../Gateway/Vec3.h"

namespace Gateway
{

class Sphere
{
public:
	const Vec3& GetCenter() const { return m_center; }

	float		GetRadius() const { return m_radius; }

public:
	Sphere(const Vec3& center, float radius);

private:
	Vec3	m_center;
	float	m_radius;
};

}