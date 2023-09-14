#include "../Gateway/Quat.h"

#include <math.h>

namespace Gateway
{

Quat Quat::operator-()
{
	Quat res;

	res.x = -x;
	res.y = -y;
	res.z = -z;
	res.w = -w;

	return res;
}

Quat& Quat::operator=(const Quat& other)
{
	w = other.w;
	x = other.x;
	y = other.y;
	z = other.z;

	return *this;
}

Quat& Quat::operator*=(const Quat& other)
{
	*this = ( *this )*other;

	return *this;
}

Quat Quat::operator*(const Quat& other) const
{
	Quat q;

	q.w = ( w*other.w )-( x*other.x )-( y*other.y )-( z*other.z );
	q.x = ( w*other.x )+( x*other.w )+( y*other.z )-( z*other.y );
	q.y = ( w*other.y )+( y*other.w )+( z*other.x )-( x*other.z );
	q.z = ( w*other.z )+( z*other.w )+( x*other.y )-( y*other.x );

	return q;
}

Vec3 Quat::operator*(const Vec3& vec) const
{
	Vec3 res;
	Vec3 qVec = { x, y, z };

	Vec3 cross1 = qVec.Cross(vec);
	Vec3 cross2 = qVec.Cross(cross1);

	res.x = vec.x+2.0f*( cross1.x*w+cross2.x );
	res.y = vec.y+2.0f*( cross1.y*w+cross2.y );
	res.z = vec.z+2.0f*( cross1.z*w+cross2.z );

	return res;
}

Quat& Quat::FromIdentity()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	return *this;
}

float Quat::Length() const
{
	return sqrtf(w*w + x*x + y*y + z*z);
}

float Quat::LengthSquared() const
{
	return w*w+x*x+y*y+z*z;
}

Quat& Quat::Normalize()
{
	const float length = Length();

	const float invLength = 1.0f/length;

	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;

	return *this;
}

Quat& Quat::FromRotation(float angle, const Vec3& globalAxis)
{
	float halfAngle_rad = ( 3.14159265358979323846264338327950288f/360.0f )*angle;

	const Vec3 N = globalAxis.Normalized();

	const float sinAngle = sinf(halfAngle_rad);
	const float cosAngle = cosf(halfAngle_rad);

	w = cosAngle;
	x = sinAngle*N.x;
	y = sinAngle*N.y;
	z = sinAngle*N.z;

	return *this;
}

Quat Quat::Inversed() const
{
	Quat res;

	res.w = w;
	res.x = -x;
	res.y = -y;
	res.z = -z;

	return res;
}

}