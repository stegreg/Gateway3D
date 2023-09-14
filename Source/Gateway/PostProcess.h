#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/World.h"

namespace Gateway
{

class Material;

class PostProcess : public RefCounted
{
	friend class Camera;

public:
	static Ref<PostProcess> Create();
	
public:
	void			SetMaterial(Ref<Material> material);

	Ref<Material>	GetMaterial();

private:
	PostProcess();

	~PostProcess();

private:
	void ExtractRenderData(RenderData::Camera& cameraRenderData);

private:
	Ref<Material> m_material;
};

}