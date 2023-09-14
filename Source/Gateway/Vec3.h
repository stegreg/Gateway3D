#pragma once

namespace Gateway
{

struct Vec3
{
	float x, y, z;

	Vec3	operator-() const;

	Vec3	operator +(const Vec3& other) const;

	Vec3&	operator +=(const Vec3& other);

	Vec3	operator -(const Vec3& other) const;

	Vec3&	operator -=(const Vec3& other);

	Vec3&	operator =(const Vec3& other);

	Vec3	operator *(const float scalar) const;

	Vec3&	operator *=(const float scalar);

	Vec3	operator /(const float scalar) const;

	Vec3&	operator /=(const float scalar);

	Vec3&	Normalize();

	Vec3	Normalized() const;

	float	Length() const;

	float	LengthSquared() const;

	Vec3	Cross(const Vec3& other) const;
};

Vec3 operator*(float scalar, const Vec3& vec);

}