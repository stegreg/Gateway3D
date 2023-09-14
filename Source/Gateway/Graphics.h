#pragma once

#include "../Gateway/Vec2.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/Mat4.h"

#include <string>
#include <vector>

namespace Gateway
{

enum
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,

	SHADER_TYPE_COUNT
};

enum : unsigned
{
	CLEAR_COLOR = 1<<0,
	CLEAR_DEPTH = 1<<1,
	CLEAR_STENCIL = 1<<2
};

struct VertexStandard
{
	Vec3  position	= { 0.0, 0.0, 0.0 };
	Vec3  normal	= { 0.0, 0.0, 0.0 };
	Vec3  tangent	= { 0.0, 0.0, 0.0 };
	fVec2 uv		= { 0.0, 0.0 };
};

class Graphics
{
public:
	static unsigned int CompileShader(
		const std::string& source, 
		unsigned int shaderType, unsigned int version, 
		const std::vector<std::string>& defines, 
		std::string& log
	);

	static unsigned int LinkProgram(
		unsigned int vertexShaderObject,
		unsigned int fragmentShaderObject,
		std::string& log
	);
};

}