#include "../Gateway/Mat4.h"
#include "../Gateway/Vec4.h"

#include <math.h>

namespace Gateway
{

Mat4& Mat4::operator=(const Mat4& other)
{
	m00 = other.m00; m01 = other.m01; m02 = other.m02; m03 = other.m03;
	m10 = other.m10; m11 = other.m11; m12 = other.m12; m13 = other.m13;
	m20 = other.m20; m21 = other.m21; m22 = other.m22; m23 = other.m23;
	m30 = other.m30; m31 = other.m31; m32 = other.m32; m33 = other.m33;

	return *this;
}

Mat4 Mat4::operator*(const Mat4& right)
{
	Mat4 res;

	res.m00 = ( m00*right.m00 )+( m01*right.m10 )+( m02*right.m20 )+( m03*right.m30 );
	res.m01 = ( m00*right.m01 )+( m01*right.m11 )+( m02*right.m21 )+( m03*right.m31 );
	res.m02 = ( m00*right.m02 )+( m01*right.m12 )+( m02*right.m22 )+( m03*right.m32 );
	res.m03 = ( m00*right.m03 )+( m01*right.m13 )+( m02*right.m23 )+( m03*right.m33 );

	res.m10 = ( m10*right.m00 )+( m11*right.m10 )+( m12*right.m20 )+( m13*right.m30 );
	res.m11 = ( m10*right.m01 )+( m11*right.m11 )+( m12*right.m21 )+( m13*right.m31 );
	res.m12 = ( m10*right.m02 )+( m11*right.m12 )+( m12*right.m22 )+( m13*right.m32 );
	res.m13 = ( m10*right.m03 )+( m11*right.m13 )+( m12*right.m23 )+( m13*right.m33 );

	res.m20 = ( m20*right.m00 )+( m21*right.m10 )+( m22*right.m20 )+( m23*right.m30 );
	res.m21 = ( m20*right.m01 )+( m21*right.m11 )+( m22*right.m21 )+( m23*right.m31 );
	res.m22 = ( m20*right.m02 )+( m21*right.m12 )+( m22*right.m22 )+( m23*right.m32 );
	res.m23 = ( m20*right.m03 )+( m21*right.m13 )+( m22*right.m23 )+( m23*right.m33 );

	res.m30 = ( m30*right.m00 )+( m31*right.m10 )+( m32*right.m20 )+( m33*right.m30 );
	res.m31 = ( m30*right.m01 )+( m31*right.m11 )+( m32*right.m21 )+( m33*right.m31 );
	res.m32 = ( m30*right.m02 )+( m31*right.m12 )+( m32*right.m22 )+( m33*right.m32 );
	res.m33 = ( m30*right.m03 )+( m31*right.m13 )+( m32*right.m23 )+( m33*right.m33 );

	return res;
}

Mat4 Mat4::operator/(const float& scalar)
{
	return Mat4{
		m00 / scalar, m01 / scalar, m02 / scalar, m03 / scalar,
		m10 / scalar, m11 / scalar, m12 / scalar, m13 / scalar,
		m20 / scalar, m21 / scalar, m22 / scalar, m23 / scalar,
		m30 / scalar, m31 / scalar, m32 / scalar, m33 / scalar,
	};
}

Mat4& Mat4::operator/=(const float& scalar)
{
	m00 /= scalar; m01 /= scalar; m02 /= scalar; m03 /= scalar;
	m10 /= scalar; m11 /= scalar; m12 /= scalar; m13 /= scalar;
	m20 /= scalar; m21 /= scalar; m22 /= scalar; m23 /= scalar;
	m30 /= scalar; m31 /= scalar; m32 /= scalar; m33 /= scalar;

	return *this;
}

Vec4 Mat4::operator*(const Vec4& vec) const
{
	Vec4 res = {
		m00*vec.x+m01*vec.y+m02*vec.z+m03*vec.w,
		m10*vec.x+m11*vec.y+m12*vec.z+m13*vec.w,
		m20*vec.x+m21*vec.y+m22*vec.z+m23*vec.w,
		m30*vec.x+m31*vec.y+m32*vec.z+m33*vec.w,
	};
	return res;
}

Mat4& Mat4::Transpose()
{
	Mat4 copy = *this;

	m00 = copy.m00; m01 = copy.m10; m02 = copy.m20; m03 = copy.m30;
	m10 = copy.m01; m11 = copy.m11; m12 = copy.m21; m13 = copy.m31;
	m20 = copy.m02; m21 = copy.m12; m22 = copy.m22; m23 = copy.m32;
	m30 = copy.m03; m31 = copy.m13; m32 = copy.m23; m33 = copy.m33;

	return *this;
}

Mat4 Mat4::Transposed() const
{
	Mat4 copy = *this;

	copy.Transpose();

	return copy;
}

Mat4& Mat4::FromIdentity()
{
	m00 = 1.0; m01 = 0.0; m02 = 0.0; m03 = 0.0;
	m10 = 0.0; m11 = 1.0; m12 = 0.0; m13 = 0.0;
	m20 = 0.0; m21 = 0.0; m22 = 1.0; m23 = 0.0;
	m30 = 0.0; m31 = 0.0; m32 = 0.0; m33 = 1.0;

	return *this;
}

Mat4& Mat4::FromFrustum(float fov, float aspect, float far, float near)
{
	float fov_rad = ( 3.14f/180.0f )*fov;

	m00 = 1.0/( aspect*tanf(fov_rad/2.0f) );	m01 = 0.0;						m02 = 0.0;							m03 = 0.0;
	m10 = 0.0;									m11 = 1.0/tanf(fov_rad/2.0f);	m12 = 0.0;							m13 = 0.0;
	m20 = 0.0;									m21 = 0.0;						m22 = ( -far-near )/( near-far );	m23 = ( 2*far*near )/( near-far );
	m30 = 0.0;									m31 = 0.0;						m32 = 1.0;							m33 = 0.0;

	return *this;
}

Mat4& Mat4::FromOrtho(float b, float t, float l, float r, float n, float f)
{
	m00 = 2.0/( r-l );							m01 = 0.0;						m02 = 0.0;							m03 = 0.0;
	m10 = 0.0;									m11 = 2.0/( t-b );				m12 = 0.0;							m13 = 0.0;
	m20 = 0.0;									m21 = 0.0;						m22 = -2.0/( f-n );					m23 = 0.0;
	m30 = -( r+l )/( r-l );						m31 = -( t+b )/( t-b );			m32 = -( f+n )/( f-n );				m33 = 1.0;

	return *this;
}

Mat4& Mat4::FromTranslation(float x, float y, float z)
{
	m00 = 1.0; m01 = 0.0; m02 = 0.0; m03 = x;
	m10 = 0.0; m11 = 1.0; m12 = 0.0; m13 = y;
	m20 = 0.0; m21 = 0.0; m22 = 1.0; m23 = z;
	m30 = 0.0; m31 = 0.0; m32 = 0.0; m33 = 1.0;

	return *this;
}

Mat4& Mat4::FromQuaternion(const Quat& quat)
{
	m00 = 1-2*quat.y*quat.y-2*quat.z*quat.z; m01 = 2*quat.x*quat.y-2*quat.w*quat.z;		m02 = 2*quat.x*quat.z+2*quat.w*quat.y;	 m03 = 0.0f;
	m10 = 2*quat.x*quat.y+2*quat.w*quat.z;	 m11 = 1-2*quat.x*quat.x-2*quat.z*quat.z;	m12 = 2*quat.y*quat.z-2*quat.w*quat.x;	 m13 = 0.0f;
	m20 = 2*quat.x*quat.z-2*quat.w*quat.y;	 m21 = 2*quat.y*quat.z+2*quat.w*quat.x;		m22 = 1-2*quat.x*quat.x-2*quat.y*quat.y; m23 = 0.0f;
	m30 = 0.0f;								 m31 = 0.0;									m32 = 0.0f;								 m33 = 1.0f;

	return *this;
}

Mat4 Mat4::Inverse() const
{
	//float v0 = m20*m31-m21*m30;
	//float v1 = m20*m32-m22*m30;
	//float v2 = m20*m33-m23*m30;
	//float v3 = m21*m32-m22*m31;
	//float v4 = m21*m33-m23*m31;
	//float v5 = m22*m33-m23*m32;

	//float i00 = ( v5*m11-v4*m12+v3*m13 );
	//float i10 = -( v5*m10-v2*m12+v1*m13 );
	//float i20 = ( v4*m10-v2*m11+v0*m13 );
	//float i30 = -( v3*m10-v1*m11+v0*m12 );

	//float invDet = 1.0f/( i00*m00+i10*m01+i20*m02+i30*m03 );

	//i00 *= invDet;
	//i10 *= invDet;
	//i20 *= invDet;
	//i30 *= invDet;

	//float i01 = -( v5*m01-v4*m02+v3*m03 )*invDet;
	//float i11 = ( v5*m00-v2*m02+v1*m03 )*invDet;
	//float i21 = -( v4*m00-v2*m01+v0*m03 )*invDet;
	//float i31 = ( v3*m00-v1*m01+v0*m02 )*invDet;

	//v0 = m10*m31-m11*m30;
	//v1 = m10*m32-m12*m30;
	//v2 = m10*m33-m13*m30;
	//v3 = m11*m32-m12*m31;
	//v4 = m11*m33-m13*m31;
	//v5 = m12*m33-m13*m32;

	//float i02 = ( v5*m01-v4*m02+v3*m03 )*invDet;
	//float i12 = -( v5*m00-v2*m02+v1*m03 )*invDet;
	//float i22 = ( v4*m00-v2*m01+v0*m03 )*invDet;
	//float i32 = -( v3*m00-v1*m01+v0*m02 )*invDet;

	//v0 = m21*m10-m20*m11;
	//v1 = m22*m10-m20*m12;
	//v2 = m23*m10-m20*m13;
	//v3 = m22*m11-m21*m12;
	//v4 = m23*m11-m21*m13;
	//v5 = m23*m12-m22*m13;

	//float i03 = -( v5*m01-v4*m02+v3*m03 )*invDet;
	//float i13 = ( v5*m00-v2*m02+v1*m03 )*invDet;
	//float i23 = -( v4*m00-v2*m01+v0*m03 )*invDet;
	//float i33 = ( v3*m00-v1*m01+v0*m02 )*invDet;

	//return Mat4{
	//	i00, i01, i02, i03,
	//	i10, i11, i12, i13,
	//	i20, i21, i22, i23,
	//	i30, i31, i32, i33
	//};

	struct Mat3
	{
		float m00, m01, m02;
		float m10, m11, m12;
		float m20, m21, m22;

		float Determinant() const
		{
			float a = ( m00*m11*m22 )+( m01*m12*m20 )+( m02*m10*m21 );
			float b = ( m00*m12*m21 )+( m01*m10*m22 )+( m02*m11*m20 );
			return a-b;
		}
	};

	Mat3 tmp = {
		125, 13, 14,
		15, 167, 17,
		18, 19, 20
	};

	float d = tmp.Determinant();

	Mat3 C00 = {
		m11, m12, m13,
		m21, m22, m23,
		m31, m32, m33
	};
	Mat3 C10 = {
		m01, m02, m03,
		m21, m22, m23,
		m31, m32, m33
	};
	Mat3 C20 = {
		m01, m02, m03,
		m11, m12, m13,
		m31, m32, m33
	};
	Mat3 C30 = {
		m01, m02, m03,
		m11, m12, m13,
		m21, m22, m23
	};

	Mat3 C01 = {
		m10, m12, m13,
		m20, m22, m23,
		m30, m32, m33
	};
	Mat3 C11 = {
		m00, m02, m03,
		m20, m22, m23,
		m30, m32, m33
	};
	Mat3 C21 = {
		m00, m02, m03,
		m10, m12, m13,
		m30, m32, m33
	};
	Mat3 C31 = {
		m00, m02, m03,
		m10, m12, m13,
		m20, m22, m23
	};

	Mat3 C02 = {
		m10, m11, m13,
		m20, m21, m23,
		m30, m31, m33
	};
	Mat3 C12 = {
		m00, m01, m03,
		m20, m21, m23,
		m30 ,m31, m33
	};
	Mat3 C22 = {
		m00, m01, m03,
		m10, m11, m13,
		m30, m31, m33
	};
	Mat3 C32 = {
		m00, m01, m03,
		m10, m11, m13,
		m20, m21, m23
	};
	
	Mat3 C03 = {
		m10, m11, m12,
		m20, m21, m22,
		m30, m31, m32
	};
	Mat3 C13 = {
		m00, m01, m02,
		m20, m21, m22,
		m30, m31, m32
	};
	Mat3 C23 = {
		m00, m01, m02,
		m10, m11, m12,
		m30, m31, m32
	};
	Mat3 C33 = {
		m00, m01, m02,
		m10, m11, m12,
		m20, m21, m22
	};

	Mat4 adjMat;
	adjMat.m00 = +C00.Determinant();
	adjMat.m10 = -C10.Determinant();
	adjMat.m20 = +C20.Determinant();
	adjMat.m30 = -C30.Determinant();

	adjMat.m01 = -C01.Determinant();
	adjMat.m11 = +C11.Determinant();
	adjMat.m21 = -C21.Determinant();
	adjMat.m31 = +C31.Determinant();

	adjMat.m02 = +C02.Determinant();
	adjMat.m12 = -C12.Determinant();
	adjMat.m22 = +C22.Determinant();
	adjMat.m32 = -C32.Determinant();

	adjMat.m03 = -C03.Determinant();
	adjMat.m13 = +C13.Determinant();
	adjMat.m23 = -C23.Determinant();
	adjMat.m33 = +C33.Determinant();

	adjMat.Transpose();

	float determinant = Determinant();

	Mat4 res = adjMat/determinant;

	return res;
}

float Mat4::Determinant() const
{
	float a = m00*(
		+( m11*( ( m22*m33 )-( m23*m32 ) ) )
		+( m12*( ( m23*m31 )-( m21*m33 ) ) )
		+( m13*( ( m21*m32 )-( m22*m31 ) ) )
		);
	float b = m01*( m10*( m22*m33-m23*m32 )+m12*( m23*m30-m20*m33 )+m13*( m20*m32-m22*m30 ) );
	float c = m02*( m10*( m21*m33-m23*m31 )+m11*( m23*m30-m20*m33 )+m13*( m20*m31-m21*m30 ) );
	float d = m03*( m10*( m21*m32-m22*m31 )+m11*( m22*m30-m20*m32 )+m12*( m20*m31-m21*m30 ) );

	float res = a-b+c-d;
	return res;
}

}