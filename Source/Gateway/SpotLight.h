#pragma once

#include "../Gateway/Light.h"

namespace Gateway
{

class SpotLight : public Light
{
	GW_SIMOBJECT(SpotLight);

public:
	static Ref<SpotLight> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void	SetRange(float range);

	float	GetRange() const;

	void	SetOuterAngle(float angle);
	float	GetOuterAngle() const;
	
	void	SetInnerAngle(float angle);
	float	GetInnerAngle() const;

	void	SetFalloffFactor(float factor);
	float	GetFalloffFactor() const;

protected:
	SpotLight(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

private:
	float m_outerAngle = 45.0;
	float m_innerAngle = 20.0;

	RenderData::SpotLight m_spotLightRenderData;
};

}