#include "../Gateway/ModelInstance.h"

namespace Gateway
{

Ref<ModelInstance> ModelInstance::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	return Ref<ModelInstance>(new ModelInstance(world, name, id, parent));
}

ModelInstance::ModelInstance(World* world, const std::string& name, unsigned long long id, SimObject* parent)
	: VisualObject(world, name, id, parent)
{
}

void ModelInstance::SetModel(Ref<Model> model)
{
	m_model = model;
	m_aabb	= model->m_aabb;
}

void ModelInstance::SetMaterial(Ref<Material> material, unsigned int meshIndex)
{
	m_materials[meshIndex] = material;
}

void ModelInstance::SetOverrideMaterial(Ref<Material> material)
{
	m_overrideMaterial = material;
}

void ModelInstance::OnExtractFrameRenderData(RenderData& renderData)
{
	if ( !m_model||!m_overrideMaterial )
		return;

	for ( int i = 0; i<m_model->GetNumMeshes(); i++ )
	{
		RenderData::Mesh		mesh;
		RenderData::Material	material;

		m_model->ExtractRenderMesh(mesh, i);
		m_overrideMaterial->ExtractRenderMaterial(material);

		unsigned int lodIndex = 0;

		unsigned int batchKey = RenderData::ComputeBatchKey(mesh, lodIndex, material);
		
		RenderData::Batch& batch = renderData.batches[batchKey];
		batch.mesh				 = mesh;
		batch.material			 = material;
		batch.lodIndex			 = lodIndex;
		batch.transformsDirty	 = true;
		batch.numInstances++;
		batch.transforms.push_back(m_worldTransform);
	}
}

void ModelInstance::OnSave(toml::value& value)
{
	VisualObject::OnSave(value);

	value["model-path"]		= m_model->GetFilePath();
	value["material-path"]	= m_overrideMaterial->GetFilePath();
}

void ModelInstance::OnLoad(toml::value& value)
{
	VisualObject::OnLoad(value);

	std::string modelPath		= toml::find<std::string>(value, "model-path");
	std::string materialPath	= toml::find<std::string>(value, "material-path");

	m_model				= Model::Load(modelPath);
	m_overrideMaterial	= Material::Load(materialPath);
}

}