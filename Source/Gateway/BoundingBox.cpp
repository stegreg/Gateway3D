#include "../Gateway/BoundingBox.h"
#include "../Gateway/Sphere.h"

#include <array>

namespace Gateway
{
BoundingBox BoundingBox::FromPoints(const Vec3* points, unsigned int count)
{
	if ( count==0 )
		return BoundingBox();

	Vec3 currentMin = points[0];
	Vec3 currentMax = points[0];
	for ( unsigned int p = 1; p<count; p++ )
	{
		currentMin.x = std::min(points[p].x, currentMin.x);
		currentMin.y = std::min(points[p].y, currentMin.y);
		currentMin.z = std::min(points[p].z, currentMin.z);

		currentMax.x = std::max(points[p].x, currentMax.x);
		currentMax.y = std::max(points[p].y, currentMax.y);
		currentMax.z = std::max(points[p].z, currentMax.z);
	}

	return FromMaxMin(currentMax, currentMin);
}

BoundingBox BoundingBox::FromMaxMin(const Vec3& max, const Vec3& min)
{
	return BoundingBox(max, min);
}

float BoundingBox::SquaredDistanceToPoint(const Vec3& point)
{
	float dx = std::max(std::max(m_min.x-point.x, 0.0f), point.x-m_max.x);
	float dy = std::max(std::max(m_min.y-point.y, 0.0f), point.y-m_max.y);
	float dz = std::max(std::max(m_min.z-point.z, 0.0f), point.z-m_max.z);

	return ( dx*dx )+( dy*dy )+( dz*dz );
}

bool BoundingBox::IntersectsSphere(const Sphere& sphere)
{
	return IntersectsSphere(sphere.GetCenter(), sphere.GetRadius());
}

bool BoundingBox::IntersectsSphere(const Vec3& center, float radius)
{
	return SquaredDistanceToPoint(center)<=radius*radius;
}

BoundingBox BoundingBox::Transform(const Mat4& transform) const
{
	/*std::array<Vec3, 8> vertices =
	{
		( transform*Vec4{m_min.x, m_min.y, m_min.z, 1.0} ).XYZ(),
		( transform*Vec4{m_max.x, m_min.y, m_min.z, 1.0} ).XYZ(),
		( transform*Vec4{m_max.x, m_max.y, m_min.z, 1.0} ).XYZ(),
		( transform*Vec4{m_min.x, m_max.y, m_min.z, 1.0} ).XYZ(),

		( transform*Vec4{m_min.x, m_min.y, m_max.z, 1.0} ).XYZ(),
		( transform*Vec4{m_max.x, m_min.y, m_max.z, 1.0} ).XYZ(),
		( transform*Vec4{m_max.x, m_max.y, m_max.z, 1.0} ).XYZ(),
		( transform*Vec4{m_min.x, m_max.y, m_max.z, 1.0} ).XYZ(),
	};

	return BoundingBox::FromPoints(vertices);*/

	return BoundingBox();
}

BoundingBox::BoundingBox(const Vec3& max, const Vec3& min)
	: m_max{ max }, m_min{ min }
{
}

}