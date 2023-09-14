#include "../Gateway/PostProcess.h"
#include "../Gateway/Material.h"

namespace Gateway
{

Ref<PostProcess> PostProcess::Create()
{
	return new PostProcess;
}

void PostProcess::SetMaterial(Ref<Material> material)
{
	m_material = material;
}

Ref<Material> PostProcess::GetMaterial()
{
	return m_material;
}

PostProcess::PostProcess()
	: m_material(nullptr)
{
}

PostProcess::~PostProcess()
{
}

void PostProcess::ExtractRenderData(RenderData::Camera& cameraRenderData)
{
	RenderData::PostProcess postProcessData;

	m_material->ExtractRenderMaterial(postProcessData.material);

	cameraRenderData.postProcessChain.push_back(postProcessData);
}

}