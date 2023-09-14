#pragma once

#include "../Gateway/ModelInstance.h"
#include "../Gateway/Mat4.h"

#include <vector>
#include <map>

namespace Gateway
{

class MultiModelInstance : public ModelInstance
{
	GW_SIMOBJECT(MultiModelInstance);

public:
	static Ref<MultiModelInstance> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void SetNumTransforms(const unsigned int numTransforms);

	void SetTransform(const unsigned int index, const Mat4& transform);

	void ClearTransforms();

private:
	MultiModelInstance(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnSave(toml::value& value) override;

private:
	std::vector<Mat4> m_instanceTransforms;
	bool			  m_instanceTransformsDirty = true;
};

}