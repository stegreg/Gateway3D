#pragma once

#include "../Gateway/Light.h"

namespace Gateway
{

class AmbientLight : public Light
{
	GW_SIMOBJECT(AmbientLight);

public:
	static Ref<AmbientLight> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	AmbientLight(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

protected:
	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnLoad(toml::value& value) override;

	virtual void OnSave(toml::value& value) override;
};

}