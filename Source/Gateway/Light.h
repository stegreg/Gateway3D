#pragma once

#include "../Gateway/VisualObject.h"
#include "../Gateway/Vec3.h"

namespace Gateway
{

class Light : public VisualObject
{
public:
	void	SetIntensity(float intensity);

	float	GetIntensity() const;

	void	SetColor(const Vec3& color);

	Vec3	GetColor() const;

protected:
	Light(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

	virtual void OnExtractFrameRenderData(RenderData& renderData) override;

	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

protected:
	RenderData::Light m_baseLightRenderData;
};

}