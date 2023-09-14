#include "../Gateway/SpotLight.h"

namespace Gateway
{

Ref<SpotLight> SpotLight::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	return Ref<SpotLight>(new SpotLight(world, name, id, parent));
}

void SpotLight::SetRange(float range)
{
	m_spotLightRenderData.range = range;
}

float SpotLight::GetRange() const
{
	return m_spotLightRenderData.range;
}

void SpotLight::SetOuterAngle(float angle)
{
	m_outerAngle = angle;

	float angleRads = 3.14*angle/180.0;

	m_spotLightRenderData.outerCosAngle = std::cosf(angleRads);
}

float SpotLight::GetOuterAngle() const
{
	return m_outerAngle;
}

void SpotLight::SetInnerAngle(float angle)
{
	m_innerAngle = angle;

	float angleRads = 3.14*angle/180.0;

	m_spotLightRenderData.innerCosAngle = std::cosf(angleRads);
}

float SpotLight::GetInnerAngle() const
{
	return m_innerAngle;
}

void SpotLight::SetFalloffFactor(float factor)
{
	m_spotLightRenderData.falloffFactor = factor;
}

float SpotLight::GetFalloffFactor() const
{
	return m_spotLightRenderData.falloffFactor;
}

SpotLight::SpotLight(World* world, const std::string& name, unsigned long long id, SimObject* parent)
	: Light(world, name, id, parent)
{
}

void SpotLight::OnExtractFrameRenderData(RenderData& renderData)
{
	RenderData::SpotLight& light = renderData.spotLights.emplace_back();
	
	light.color			= m_baseLightRenderData.color;
	light.intensity		= m_baseLightRenderData.intensity;
	light.worldPos		= m_position;
	light.range			= m_spotLightRenderData.range;
	light.outerCosAngle = m_spotLightRenderData.outerCosAngle;
	light.innerCosAngle = m_spotLightRenderData.innerCosAngle;
	light.falloffFactor	= m_spotLightRenderData.falloffFactor;
	light.direction		= GetForward();
}

void SpotLight::OnSave(toml::value& value)
{
	Light::OnSave(value);

	value["range"]			= GetRange();
	value["outer-angle"]	= GetOuterAngle();
	value["inner-angle"]	= GetInnerAngle();
	value["falloff-factor"] = GetFalloffFactor();
}

void SpotLight::OnLoad(toml::value& value)
{
	Light::OnLoad(value);

	float range			= toml::find<float>(value, "range");
	float outerAngle	= toml::find<float>(value, "outer-angle");
	float innerAngle	= toml::find<float>(value, "inner-angle");
	float falloffFactor = toml::find<float>(value, "falloff-factor");

	SetRange(range);
	SetOuterAngle(outerAngle);
	SetInnerAngle(innerAngle);
	SetFalloffFactor(falloffFactor);
}

}