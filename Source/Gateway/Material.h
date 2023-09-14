#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Graphics.h"
#include "../Gateway/Shader.h"
#include "../Gateway/World.h"
#include "../Gateway/Vec2.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Texture.h"
#include "../Gateway/Vec4.h"

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>

namespace Gateway
{

class Material : public RefCounted
{
	friend class ModelInstance;
	friend class MultiModelInstance;
	friend class PostProcess;

public:
	static Ref<Material> Create();

	static Ref<Material> Load(const std::string& filepath);

public:
	void				SetShader(Ref<Shader> shader);

	Ref<Shader>			GetShader();

	bool				Link();

	void				SetTexture(const std::string& name, Ref<Texture> param, unsigned int slot);

	void				SetConstant(const std::string& name, float param);

	void				SetConstant(const std::string& name, const fVec2& param);

	void				SetConstant(const std::string& name, const Vec3& param);

	const std::string&	GetFilePath() const { return m_filepath; }

private:
	Material();

	~Material();

private:
	void ExtractRenderMaterial(RenderData::Material& renderMaterial);

private:
	std::string			 m_filepath;

	Ref<Shader>			 m_shader;

	RenderData::Material m_renderMaterial;
};


}