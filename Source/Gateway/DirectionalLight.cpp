#include "../Gateway/DirectionalLight.h"

namespace Gateway
{

Ref<DirectionalLight> DirectionalLight::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
    return Ref<DirectionalLight>(new DirectionalLight(world, name, id, parent));
}

DirectionalLight::DirectionalLight(World* world, const std::string& name, unsigned long long id, SimObject* parent)
    : Light(world, name, id, parent)
{
}

void DirectionalLight::OnExtractFrameRenderData(RenderData& renderData)
{
    RenderData::DirectionalLight& light = renderData.directionalLights.emplace_back();

    light.color     = m_baseLightRenderData.color;
    light.intensity = m_baseLightRenderData.intensity;
    light.direction = GetForward();
}

void DirectionalLight::OnSave(toml::value& value)
{
    Light::OnSave(value);

}

void DirectionalLight::OnLoad(toml::value& value)
{
    Light::OnLoad(value);

}

}