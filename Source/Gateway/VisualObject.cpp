#include "../Gateway/VisualObject.h"

namespace Gateway
{

Ref<VisualObject> VisualObject::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	return Ref<VisualObject>(new VisualObject(world, name, id, parent));
}

void VisualObject::SetPosition(float x, float y, float z, bool global)
{
	SetPosition({ x, y,z }, global);
}

void VisualObject::SetPosition(const Vec3& position, bool global)
{
	m_position = position;
}

void VisualObject::SetRotation(const Quat& rotation, bool global)
{
	m_rotation = rotation;
}

void VisualObject::Translate(const Vec3& offset)
{
	m_position += offset;
}

void VisualObject::Rotate(const Quat& rotation)
{
	m_rotation *= rotation;
}

Vec3 VisualObject::GetForward() const
{
	return m_rotation*Vec3(0.0, 0.0, 1.0);
}

Vec3 VisualObject::GetRight() const
{
	return m_rotation*Vec3(1.0, 0.0, 0.0);
}

VisualObject::VisualObject(World* world, const std::string& name, unsigned long long id, SimObject* parent)
	: SimObject(world, name, id, parent)
{
}

void VisualObject::OnSave(toml::value& value)
{
	SimObject::OnSave(value);

	value["position"]	= { toml::value(m_position.x), toml::value(m_position.y), toml::value(m_position.z) };
	value["scale"]		= { toml::value(m_scale.x), toml::value(m_scale.y), toml::value(m_scale.z) };
	value["rotation"]	= { toml::value(m_rotation.x), toml::value(m_rotation.y), toml::value(m_rotation.z), toml::value(m_rotation.w) };
}

void VisualObject::OnLoad(toml::value& value)
{
	SimObject::OnLoad(value);

	toml::value positionValue = toml::find(value, "position");
	toml::value rotationValue = toml::find(value, "rotation");
	toml::value scaleValue = toml::find(value, "scale");

	m_position.x = toml::find<float>(positionValue, 0);
	m_position.y = toml::find<float>(positionValue, 1);
	m_position.z = toml::find<float>(positionValue, 2);

	m_scale.x = toml::find<float>(scaleValue, 0);
	m_scale.y = toml::find<float>(scaleValue, 1);
	m_scale.z = toml::find<float>(scaleValue, 2);

	m_rotation.x = toml::find<float>(rotationValue, 0);
	m_rotation.y = toml::find<float>(rotationValue, 1);
	m_rotation.z = toml::find<float>(rotationValue, 2);
	m_rotation.w = toml::find<float>(rotationValue, 3);
}

void VisualObject::OnUpdate()
{
	Mat4 translation, rotation, scale;
	
	translation.FromTranslation(m_position.x, m_position.y, m_position.z);
	rotation.FromQuaternion(m_rotation);

	m_worldTransform = translation*rotation;

	m_aabb.Transform(m_worldTransform);
}


}