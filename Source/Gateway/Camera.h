#pragma once

#include "../Gateway/VisualObject.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/PostProcess.h"
#include "../Gateway/RenderTarget.h"

#include <vector>

namespace Gateway
{

enum
{
	PROJECTION_PERSPECTIVE,
	PROJECTION_ORTHOGRAPHIC
};

class Camera : public VisualObject
{
	GW_SIMOBJECT(Camera);

public:
	static Ref<Camera> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void	SetProjection(int projection);

	void	SetRange(float near, float far);

	void	SetFOV(float fov);

	void	SetClearColor(const Vec4& clearColor);

	Vec4	GetClearColor() const;

	void	SetClearDepth(float depth);

	float	GetClearDepth() const;

	void	SetClearStencil(int stencil);

	int		GetClearStencil() const;

	void	SetClearFlags(unsigned int flags);

	void	ChainPostProcess(Ref<PostProcess> postProcess);

	void	SetRenderTarget(Ref<RenderTarget> target);

public:
	Camera(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

	~Camera() override;

private:
	virtual void OnExtractFrameRenderData(RenderData& renderData) override;
		
	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

private:
	RenderData::Camera m_renderData;

	Ref<RenderTarget> m_renderTarget;

	std::vector<Ref<PostProcess>> m_postProcessChain;
};

}