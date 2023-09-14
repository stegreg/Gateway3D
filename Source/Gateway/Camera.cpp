#include "../Gateway/Camera.h"
#include "../Gateway/Window.h"
#include "../Gateway/World.h"
#include "../Gateway/Mat4.h"

#include <glad/glad.h>

#include <iostream>

namespace Gateway
{

Ref<Camera> Camera::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	return Ref<Camera>(new Camera(world, name, id, parent));
}

Camera::Camera(World* world, const std::string& name, unsigned long long id, SimObject* parent)
	: VisualObject(world, name, id, parent)
{
	SetProjection(PROJECTION_PERSPECTIVE);
	SetRange(0.1, 1000.0);
	SetFOV(80.0);
}

Camera::~Camera()
{
}

void Camera::SetProjection(int projection)
{
	m_renderData.projectionType = projection;
}

void Camera::SetRange(float near, float far)
{
	m_renderData.nearPlane	= near;
	m_renderData.farPlane	= far;
}

void Camera::SetFOV(float fov)
{
	m_renderData.fov = fov;
}

void Camera::SetClearColor(const Vec4& clearColor)
{
	m_renderData.clearColor = clearColor;
}

Vec4 Camera::GetClearColor() const
{
	return m_renderData.clearColor;
}

void Camera::SetClearDepth(float depth)
{
	m_renderData.clearDepth = depth;
}

float Camera::GetClearDepth() const
{
	return m_renderData.clearDepth;
}

void Camera::SetClearStencil(int stencil)
{
	m_renderData.clearStencil = stencil;
}

int Camera::GetClearStencil() const
{
	return m_renderData.clearStencil;
}

void Camera::SetClearFlags(unsigned int flags)
{
	m_renderData.clearFlags = flags;
}

void Camera::ChainPostProcess(Ref<PostProcess> postProcess)
{
	m_postProcessChain.push_back(postProcess);
}

void Camera::SetRenderTarget(Ref<RenderTarget> target)
{
	m_renderTarget = target;
}

void Camera::OnExtractFrameRenderData(RenderData& renderData)
{
	if ( !m_renderTarget )
		return;

	Mat4 translation, rotation;
	translation.FromTranslation(-m_position.x, -m_position.y, -m_position.z);
	rotation.FromQuaternion(m_rotation.Inversed());

	iVec2 targetSize = m_renderTarget->GetSize();

	m_renderData.projection.FromFrustum(m_renderData.fov, (float)targetSize.x/targetSize.y, m_renderData.nearPlane, m_renderData.farPlane);
	m_renderData.view = rotation*translation;
	m_renderData.worldPosition = m_position;

	m_renderData.postProcessChain.clear();
	for ( auto postProcess:m_postProcessChain )
		postProcess->ExtractRenderData(m_renderData);

	m_renderData.target.fbo  = m_renderTarget->GetFBO();
	m_renderData.target.size = targetSize;

	renderData.camera = m_renderData;
}

void Camera::OnSave(toml::value& value)
{
	VisualObject::OnSave(value);

	value["fov"]			 = toml::value(m_renderData.fov);
	value["far-plane"]		 = toml::value(m_renderData.farPlane);
	value["near-plane"]		 = toml::value(m_renderData.nearPlane);
	value["projection-type"] = toml::value(m_renderData.projectionType);
	value["clear-color"]	 = toml::value{ m_renderData.clearColor.x, m_renderData.clearColor.y, m_renderData.clearColor.z, m_renderData.clearColor.w };
	value["clear-depth"]	 = toml::value(m_renderData.clearDepth);
	value["clear-stencil"]	 = toml::value(m_renderData.clearStencil);
	value["clear-flags"]	 = toml::value(m_renderData.clearFlags);
}

void Camera::OnLoad(toml::value& value)
{
	VisualObject::OnLoad(value);

	m_renderData.fov			= toml::find<float>(value, "fov");
	m_renderData.nearPlane		= toml::find<float>(value, "near-plane");
	m_renderData.farPlane		= toml::find<float>(value, "far-plane");

	std::vector<float> clearColorValue = toml::find<std::vector<float>>(value, "clear-color");

	m_renderData.clearColor.x = clearColorValue.at(0);
	m_renderData.clearColor.y = clearColorValue.at(1);
	m_renderData.clearColor.z = clearColorValue.at(2);
	m_renderData.clearColor.w = clearColorValue.at(3);

	m_renderData.clearDepth		= toml::find<float>(value, "clear-depth");
	m_renderData.clearStencil	= toml::find<int>(value, "clear-stencil");
	m_renderData.clearFlags		= toml::find<unsigned int>(value, "clear-flags");
}

}