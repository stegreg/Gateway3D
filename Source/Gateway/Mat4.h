#pragma once

#include "../Gateway/Quat.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Vec2.h"

namespace Gateway
{

struct Vec4;

struct Mat4
{
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

	Mat4& operator=(const Mat4& other);

	Mat4  operator*(const Mat4& other);

	Mat4 operator/(const float& scalar);

	Mat4& operator/=(const float& scalar);

	Vec4 operator*(const Vec4& vec) const;

	Mat4& Transpose();

	Mat4  Transposed() const;

	Mat4& FromIdentity();

	Mat4& FromFrustum(float fov, float aspect, float near, float far);

	Mat4& FromOrtho(float b, float t, float l, float r, float n, float f);

	Mat4& FromTranslation(float x, float y, float z);

	Mat4& FromQuaternion(const Quat& quat);

	Mat4  Inverse() const;

	float Determinant() const;
};

}