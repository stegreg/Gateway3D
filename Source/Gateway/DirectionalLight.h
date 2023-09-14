#pragma once

#include "../Gateway/Light.h"

namespace Gateway
{

class DirectionalLight : public Light
{
	GW_SIMOBJECT(DirectionalLight);

public:
	static Ref<DirectionalLight> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

protected:
	DirectionalLight(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

protected:
	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

private:
	RenderData::DirectionalLight m_directionalLightRenderData;
};

}