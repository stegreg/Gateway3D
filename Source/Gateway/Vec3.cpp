#include "../Gateway/Vec3.h"

#include <math.h>

namespace Gateway
{


Vec3 Vec3::operator-() const
{
	Vec3 res;

	res.x = -x;
	res.y = -y;
	res.z = -z;
	
	return res;
}

Vec3 Vec3::operator+(const Vec3& other) const
{
	return Vec3(x+other.x, y+other.y, z+other.z);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

Vec3 Vec3::operator-(const Vec3& other) const
{
	return Vec3(x-other.x, y-other.y, z-other.z);
}

Vec3& Vec3::operator-=(const Vec3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}
Vec3& Vec3::operator=(const Vec3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;

	return *this;
}

Vec3 Vec3::operator *(const float scalar) const
{
	Vec3 res;

	res.x = x * scalar;
	res.y = y * scalar;
	res.z = z * scalar;
	
	return res;
}

Vec3& Vec3::operator *=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

Vec3 Vec3::operator/(const float scalar) const
{
	Vec3 res;

	res.x = x/scalar;
	res.y = y/scalar;
	res.z = z/scalar;

	return res;
}

Vec3& Vec3::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}

Vec3& Vec3::Normalize()
{
	const float length = Length();

	x /= length;
	y /= length;
	z /= length;

	return *this;
}

Vec3 Vec3::Normalized() const
{
	Vec3 res;

	const float length = Length();

	res.x = x/length;
	res.y = y/length;
	res.z = z/length;

	return res;
}

float Vec3::Length() const
{
	return sqrtf(x*x+y*y+z*z);
}

float Vec3::LengthSquared() const
{
	return x*x+y*y+z*z;
}

Vec3 Vec3::Cross(const Vec3& other) const
{
	Vec3 res;

	res.x = y*other.z-z*other.y;
	res.y = z*other.x-x*other.z;
	res.z = x*other.y-y*other.x;

	return res;
}

Vec3 operator*(float scalar, const Vec3& vec)
{
	return vec*scalar;
}

}
