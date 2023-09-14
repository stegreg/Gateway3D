#pragma once

#include "../Gateway/Light.h"

namespace Gateway
{

class PointLight : public Light
{
	GW_SIMOBJECT(PointLight);

public:
	static Ref<PointLight> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void	SetRange(float range);

	float	GetRange() const;

	void	SetFalloffFactor(float factor);

protected:
	PointLight(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

private:
	RenderData::PointLight m_pointLightRenderData;
};

}