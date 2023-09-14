#include "../Gateway/AmbientLight.h"

namespace Gateway
{

Ref<AmbientLight> AmbientLight::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
    return Ref<AmbientLight>(new AmbientLight(world, name, id, parent));
}

AmbientLight::AmbientLight(World* world, const std::string& name, unsigned long long id, SimObject* parent)
    : Light(world, name, id, parent)
{
}

void AmbientLight::OnExtractFrameRenderData(RenderData& renderData)
{
    RenderData::AmbientLight& light = renderData.ambientLights.emplace_back();

    light.color     = m_baseLightRenderData.color;
    light.intensity = m_baseLightRenderData.intensity;
}

void AmbientLight::OnLoad(toml::value& value)
{
    Light::OnLoad(value);

}

void AmbientLight::OnSave(toml::value& value)
{
    Light::OnSave(value);

}

}