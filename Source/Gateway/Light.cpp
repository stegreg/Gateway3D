#include "../Gateway/Light.h"

namespace Gateway
{

void Light::SetIntensity(float intensity)
{
    if ( intensity<0 )
        return;

    m_baseLightRenderData.intensity = intensity;
}

float Light::GetIntensity() const
{
    return m_baseLightRenderData.intensity;
}

void Light::SetColor(const Vec3& color)
{
    m_baseLightRenderData.color = color;
}

Vec3 Gateway::Light::GetColor() const
{
    return m_baseLightRenderData.color;
}

Light::Light(World* world, const std::string& name, unsigned long long id, SimObject* parent)
    : VisualObject(world, name, id, parent)
{
}

void Light::OnExtractFrameRenderData(RenderData& renderData)
{

}

void Light::OnSave(toml::value& value)
{
    VisualObject::OnSave(value);

    value["intensity"]  = toml::value(m_baseLightRenderData.intensity);
    value["color"]      = { toml::value(m_baseLightRenderData.color.x), toml::value(m_baseLightRenderData.color.y), toml::value(m_baseLightRenderData.color.z) };
}

void Light::OnLoad(toml::value& value)
{
    VisualObject::OnLoad(value);

    toml::value colorValue = toml::find(value, "color");

    m_baseLightRenderData.color.x = toml::find<float>(colorValue, 0);
    m_baseLightRenderData.color.y = toml::find<float>(colorValue, 1);
    m_baseLightRenderData.color.z = toml::find<float>(colorValue, 2);

    m_baseLightRenderData.intensity = toml::find<float>(value, "intensity");
}

}