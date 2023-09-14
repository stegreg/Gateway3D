#include "../Gateway/SimObject.h"
#include "../Gateway/World.h"

#include "../Gateway/AmbientLight.h"
#include "../Gateway/DirectionalLight.h"
#include "../Gateway/PointLight.h"
#include "../Gateway/SpotLight.h"
#include "../Gateway/Camera.h"
#include "../Gateway/ModelInstance.h"
#include "../Gateway/MultiModelInstance.h"

namespace Gateway
{

SimObject::SimObject(World* world, const std::string& name, unsigned long long id, SimObject* parent)
	: m_world(world), m_parent(parent), m_name(name), m_id(id)
{
	if ( parent )
	{
		m_parent = parent;
		parent->m_children.push_back(this);
	}

	m_world->RegisterSimObject(this);
}

SimObject::~SimObject()
{
}

void SimObject::OnWorldRegister()
{
}

void SimObject::OnWorldUnregister()
{
}

void SimObject::OnExtractFrameRenderData(RenderData& renderData)
{
}

void SimObject::OnUpdate()
{
}

void SimObject::OnSave(toml::value& value)
{
	value["type"]	= GetTypeName();
	value["name"]	= m_name;
	value["id"]		= m_id;
}

void SimObject::OnLoad(toml::value& value)
{
	m_name	= toml::find<std::string>(value, "name");
	m_id	= toml::find<unsigned long long>(value, "id");
}

Ref<SimObject> SimObject::Create(World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	return Ref<SimObject>(new SimObject(world, name, id, parent));
}

void SimObject::Delete()
{
	m_world->UnregisterSimObject(this);
}

void SimObject::SetName(const std::string& name)
{
	m_name = m_world->OnSimObjectRename(this, name);
}

const std::string& SimObject::GetName() const
{
	return m_name;
}

bool SimObject::HasName() const
{
	return !m_name.empty();
}

const unsigned long long SimObject::GetID() const
{
	return m_id;
}

std::unordered_map<std::string, Ref<SimObjectFactoryBase>> SimObjectRegistrar::s_factories;

void SimObjectRegistrar::RegisterAllTypes()
{
	RegisterType<SimObject>();
	RegisterType<VisualObject>();
	RegisterType<Camera>();
	RegisterType<AmbientLight>();
	RegisterType<DirectionalLight>();
	RegisterType<PointLight>();
	RegisterType<SpotLight>();
	RegisterType<ModelInstance>();
	RegisterType<MultiModelInstance>();
}

}
