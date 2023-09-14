#include "../Gateway/MultiModelInstance.h"

namespace Gateway
{

Ref<MultiModelInstance> MultiModelInstance::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
    return Ref<MultiModelInstance>(new MultiModelInstance(world, name, id, parent));
}

void MultiModelInstance::SetNumTransforms(const unsigned int numTransforms)
{
	m_instanceTransforms.resize(numTransforms);
}

void MultiModelInstance::SetTransform(const unsigned int index, const Mat4& transform)
{
	m_instanceTransforms.at(index) = transform;

	m_instanceTransformsDirty = true;
}

void MultiModelInstance::ClearTransforms()
{
}

MultiModelInstance::MultiModelInstance(World* world, const std::string& name, unsigned long long id, SimObject* parent)
    : ModelInstance(world, name, id, parent)
{

}

void MultiModelInstance::OnExtractFrameRenderData(RenderData& renderData)
{
	if ( !m_model||!m_overrideMaterial )
		return;

	for ( int i = 0; i<m_model->GetNumMeshes(); i++ )
	{
		RenderData::Mesh		mesh;
		RenderData::Material	material;

		m_model->ExtractRenderMesh(mesh, i);
		m_overrideMaterial->ExtractRenderMaterial(material);

		Mat4 translation, rotation, scale;
		translation.FromTranslation(m_position.x, m_position.y, m_position.z);
		rotation.FromQuaternion(m_rotation);
		Mat4 transform = translation*rotation;

		unsigned int lodIndex = 0;

		unsigned int batchKey = RenderData::ComputeBatchKey(mesh, lodIndex, material);

		RenderData::Batch& batch = renderData.batches[batchKey];
		batch.mesh				 = mesh;
		batch.material			 = material;
		batch.lodIndex			 = lodIndex;
		batch.transformsDirty	 = m_instanceTransformsDirty;
		batch.numInstances		+= m_instanceTransforms.size();
		batch.transforms.insert(batch.transforms.cbegin(), m_instanceTransforms.cbegin(), m_instanceTransforms.cend());
	}
}

void MultiModelInstance::OnSave(toml::value& value)
{
	ModelInstance::OnSave(value);
	
	toml::array transformsArray;
	for ( const auto& transform:m_instanceTransforms )
	{
		transformsArray.push_back(
			{
				toml::value(transform.m00), toml::value(transform.m10), toml::value(transform.m20), toml::value(transform.m30),
				toml::value(transform.m01), toml::value(transform.m11), toml::value(transform.m21), toml::value(transform.m31),
				toml::value(transform.m02), toml::value(transform.m12), toml::value(transform.m22), toml::value(transform.m32),
				toml::value(transform.m03), toml::value(transform.m13), toml::value(transform.m23), toml::value(transform.m33)
			}
		);
	}

	value["transforms"] = transformsArray;
}

}