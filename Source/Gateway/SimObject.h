#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Transform.h"
#include "../Gateway/World.h"

#include <toml.hpp>

#include <vector>
#include <string>
#include <list>
#include <unordered_map>

#define GW_SIMOBJECT(classtype)												\
public:																		\
	static  std::string GetTypeNameStatic() { return #classtype; }			\
	virtual std::string GetTypeName()		{ return GetTypeNameStatic(); }

namespace Gateway
{

class SimObject : public RefCounted
{
	GW_SIMOBJECT(SimObject);

	friend class World;

public:
	static Ref<SimObject>		Create(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void						Delete();

	void						SetName(const std::string& name);

	const std::string&			GetName() const;

	bool						HasName() const;

	const unsigned long long	GetID() const;

protected:
	virtual void OnWorldRegister();

	virtual void OnWorldUnregister();

	virtual void OnExtractFrameRenderData(RenderData& renderData);

	virtual void OnUpdate();

	virtual void OnSave(toml::value& value);

	virtual void OnLoad(toml::value& value);

protected:
	SimObject(World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);
	
	virtual ~SimObject();

protected:
	World*						m_world;

	std::string					m_name;

	unsigned long long			m_id;

	SimObject*					m_parent;

	std::list<Ref<SimObject>>	m_children;
};

class SimObjectFactoryBase : public RefCounted
{
public:
	virtual Ref<SimObject> Create(World* world, const std::string& name, unsigned long long id, SimObject* parent) = 0;
};

template<class T>
class SimObjectFactory : public SimObjectFactoryBase
{
public:
	Ref<SimObject> Create(World* world, const std::string& name, unsigned long long id, SimObject* parent) override
	{
		return (T::Create(world, name, id, parent)).Cast<SimObject>();
	}
};

class SimObjectRegistrar
{
public:
	template<class T>
	static void				RegisterType();

	static Ref<SimObject>	CreateSimObject(const std::string& typeName, World* world, const std::string& name, unsigned long long id, SimObject* parent);

	static void				RegisterAllTypes();

private:
	static std::unordered_map<std::string, Ref<SimObjectFactoryBase>> s_factories;
};

template<class T>
void SimObjectRegistrar::RegisterType()
{
	std::string typeStr = T::GetTypeNameStatic();

	if ( s_factories.contains(typeStr) )
		return;

	s_factories[typeStr] = Ref<SimObjectFactoryBase>(new SimObjectFactory<T>());
}

inline Ref<SimObject> SimObjectRegistrar::CreateSimObject(const std::string& typeName, World* world, const std::string& name, unsigned long long id, SimObject* parent)
{
	if ( !s_factories.contains(typeName) )
		return nullptr;

	return s_factories.at(typeName)->Create(world, name, id, parent);
}

}