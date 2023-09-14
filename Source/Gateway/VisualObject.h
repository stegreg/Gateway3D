#pragma once

#include "../Gateway/SimObject.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Quat.h"
#include "../Gateway/Mat4.h"
#include "../Gateway/BoundingBox.h"

namespace Gateway
{

class VisualObject : public SimObject
{
	GW_SIMOBJECT(VisualObject);

public:
	static Ref<VisualObject> Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void	SetPosition(float x, float y, float z, bool global = false);

	void	SetPosition(const Vec3& position, bool global = false);

	void	SetRotation(const Quat& rotation, bool global = false);

	const Vec3& GetPosition() const { return m_position; }

	void	Translate(const Vec3& offset);

	void	Rotate(const Quat& rotation);

	Vec3	GetForward() const;

	Vec3	GetRight() const;

	const BoundingBox& GetAABB() const { return m_aabb; }

protected:
	VisualObject(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

protected:
	virtual void OnSave(toml::value& value) override;

	virtual void OnLoad(toml::value& value) override;

	virtual void OnUpdate() override;

protected:
	Vec3		m_position	= { 0.0, 0.0, 0.0 };

	Vec3		m_scale		= { 1.0, 1.0, 1.0 };

	Quat		m_rotation	= { 0.0, 0.0, 0.0, 1.0 };

	Mat4		m_worldTransform;

	BoundingBox m_aabb;
};

}