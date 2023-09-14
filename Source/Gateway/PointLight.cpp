#include "../Gateway/PointLight.h"

#include "../Gateway/World.h"

namespace Gateway
{

Ref<PointLight> PointLight::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	return Ref<PointLight>(new PointLight(world, name, id, parent));
}

void PointLight::SetRange(float range)
{
	m_pointLightRenderData.range = range;
}

float PointLight::GetRange() const
{
	return m_pointLightRenderData.range;
}

void PointLight::SetFalloffFactor(float exponent)
{
	m_pointLightRenderData.fallofFactor = exponent;
}

PointLight::PointLight(World* world, const std::string& name, unsigned long long id, SimObject* parent)
	: Light(world, name, id, parent)
{
}

void PointLight::OnExtractFrameRenderData(RenderData& renderData)
{
	RenderData::PointLight& light = renderData.pointLights.emplace_back();

	light.color			= m_baseLightRenderData.color;
	light.intensity		= m_baseLightRenderData.intensity;
	light.worldPos		= m_position;
	light.range			= m_pointLightRenderData.range;
	light.fallofFactor	= m_pointLightRenderData.fallofFactor;
}

void PointLight::OnSave(toml::value& value)
{
	Light::OnSave(value);

	value["range"]			= m_pointLightRenderData.range;
	value["falloff-factor"] = m_pointLightRenderData.fallofFactor;
}

void PointLight::OnLoad(toml::value& value)
{
	Light::OnLoad(value);

	m_pointLightRenderData.range		= toml::find<float>(value, "range");
	m_pointLightRenderData.fallofFactor = toml::find<float>(value, "falloff-factor");
}

}