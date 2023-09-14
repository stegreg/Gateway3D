#pragma once

#include "../Gateway/VisualObject.h"
#include "../Gateway/Material.h"
#include "../Gateway/Model.h"

#include <vector>
#include <map>

namespace Gateway
{

class ModelInstance : public VisualObject
{
	GW_SIMOBJECT(ModelInstance);

public:
	static Ref<ModelInstance> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void SetModel(Ref<Model> model);

	/// TODO: Not supported yet. Use SetOverrideMaterial()
	void SetMaterial(Ref<Material> material, unsigned int meshIndex);

	void SetOverrideMaterial(Ref<Material> material);

public:
	ModelInstance(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

protected:
	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

protected:
	Ref<Model>							   m_model;

	/// TODO: Not supported yet
	std::map<unsigned int, Ref<Material>>  m_materials;
	
	Ref<Material>						   m_overrideMaterial;
};

}