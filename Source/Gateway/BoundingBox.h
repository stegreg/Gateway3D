#pragma once
#include "../Gateway/Vec4.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Mat4.h"

#include <array>

namespace Gateway
{

class Sphere;
class Mat4;

class BoundingBox
{
public:
	template<unsigned int N>
	static BoundingBox FromPoints(const std::array<Vec3, N>& points);

	static BoundingBox FromPoints(const Vec3* points, unsigned int count);
	
	static BoundingBox FromMaxMin(const Vec3& max, const Vec3& min);

public:
	float	SquaredDistanceToPoint(const Vec3& point);

	bool	IntersectsSphere(const Sphere& sphere);

	bool	IntersectsSphere(const Vec3& center, float radius);

	BoundingBox Transform(const Mat4& transform) const;

	const Vec3& GetMax() const { return m_max; }

	const Vec3& GetMin() const { return m_min; }

public:
	BoundingBox(const Vec3& max = { 0.0, 0.0, 0.0 }, const Vec3& min = { 0.0, 0.0, 0.0 });

private:
	Vec3 m_max, m_min;
};

template<unsigned int N>
BoundingBox BoundingBox::FromPoints(const std::array<Vec3, N>& points)
{
	return FromPoints(points.data(), points.size());
}

}