#include "../Gateway/World.h"

#include "../Gateway/RenderPath.h"

#include "../Gateway/SimObject.h"
#include "../Gateway/Window.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/Camera.h"
#include "../Gateway/ModelInstance.h"
#include "../Gateway/MultiModelInstance.h"
#include "../Gateway/AmbientLight.h"
#include "../Gateway/DirectionalLight.h"
#include "../Gateway/PointLight.h"
#include "../Gateway/SpotLight.h"

#include <glad/glad.h>

#include <SDL2/SDL.h>

#include <toml.hpp>

#include <algorithm>
#include <iostream>

namespace Gateway
{

void World::RegisterSimObject(Ref<SimObject> simObj)
{
    m_simObjects.push_back(simObj);

    unsigned long long id = simObj->GetID();

    if ( m_simObjectsByID.contains(id)||id==0 )
    {
        id = ++m_idGenerator;

        // Correct the SimObject's id
        simObj->m_id = id;
    }

    m_simObjectsByID[id] = simObj;

    if ( simObj->HasName() )
    {
        std::string name = simObj->GetName();

        if ( m_simObjectsByName.contains(name) )
        {
            name += "_"+std::to_string(id);

            // Correct the SimObjects' name
            simObj->m_name = name;
        }

        m_simObjectsByName[name] = simObj;
    }
}

void World::UnregisterSimObject(Ref<SimObject> simObj)
{
    {
        auto it = std::find(m_simObjects.begin(), m_simObjects.end(), simObj);

        if ( it==m_simObjects.end() )
            return;

        m_simObjects.erase(it);
    }

    {
        unsigned long long id = simObj->GetID();

        m_simObjectsByID.erase(id);
    }

    {
        if ( simObj->HasName() )
            m_simObjectsByName.erase(simObj->GetName());
    }
}

std::string World::OnSimObjectRename(Ref<SimObject> simObj, const std::string& newName)
{
    std::string name = newName;

    if ( simObj->HasName() )
    {
        m_simObjectsByName.erase(simObj->GetName());

        if ( m_simObjectsByName.contains(name) )
        {
            unsigned long long id = simObj->GetID();
            name += "_"+std::to_string(id);
        }
    }

    m_simObjectsByName[name] = simObj;

    return name;
}

Ref<World> World::Create()
{
    return Ref<World>(new World);
}

Ref<World> World::Load(const std::string& filepath)
{
    Ref<World> world = Create();

    toml::value root = toml::parse(FileSystem::GetFullPath(filepath));
    toml::value worldValue = root.at("world");

    toml::array simObjectsArray = worldValue.at("sim-object").as_array();

    for ( auto simObjectValue:simObjectsArray )
    {
        std::string simObjectTypeName   = toml::find<std::string>(simObjectValue, "type");
        std::string simObjectName       = toml::find<std::string>(simObjectValue, "name");
        unsigned long long simObjectID  = toml::find<unsigned long long>(simObjectValue, "id");
        
        Ref<SimObject> simObject = SimObjectRegistrar::CreateSimObject(simObjectTypeName, world, simObjectName, simObjectID, nullptr);

        simObject->OnLoad(simObjectValue);
    }

    return world;
}

bool World::Save(Ref<World> world, const std::string& filepath)
{
    std::ofstream outFile(FileSystem::GetFullPath(filepath).c_str());

    if ( !outFile.is_open() )
        return false;

    toml::value root;
    toml::value worldValue;

    // Save SimObjects
    {
        toml::array simObjectsArray;

        for ( auto obj:world->m_simObjects )
        {
            toml::value simObjValue;

            obj->OnSave(simObjValue);

            simObjectsArray.push_back(simObjValue);
        }

        worldValue["sim-object"] = simObjectsArray;
    }

    root["world"] = worldValue;
    
    outFile<<std::setw(70)<<root;

    outFile.close();
    return true;
}

void World::Update()
{
    for ( auto obj:m_simObjects )
        obj->OnUpdate();
}

Ref<SimObject> World::GetSimObjectByID(const unsigned long long id)
{
    if ( m_simObjectsByID.contains(id) )
        return m_simObjectsByID.at(id);

    return nullptr;
}

Ref<SimObject> World::GetSimObjectByName(const std::string& name)
{
    if ( m_simObjectsByName.contains(name) )
        return m_simObjectsByName.at(name);

    return nullptr;
}

RenderData& World::PrepareRenderData()
{
    m_extractedRenderData.dirty = true;
    m_extractedRenderData.ambientLights.clear();
    m_extractedRenderData.directionalLights.clear();
    m_extractedRenderData.spotLights.clear();
    m_extractedRenderData.pointLights.clear();
    m_extractedRenderData.batches.clear();
    m_extractedRenderData.rmlContexts.clear();
    m_extractedRenderData.camera.postProcessChain.clear();

    return m_extractedRenderData;
}

RenderData& World::ExtractRenderData()
{
    if ( m_extractedRenderData.dirty )
    {
        for ( auto obj:m_simObjects )
            obj->OnExtractFrameRenderData(m_extractedRenderData);
    }

    m_extractedRenderData.dirty = false;

    return m_extractedRenderData;
}

World::World()
{
}

World::~World()
{
}

void RenderData::Material::BindDataToProgram(unsigned int program)
{
    for ( auto entry:textures.tex1d )
    {
        glActiveTexture(GL_TEXTURE0+entry.first);
        glBindTexture(GL_TEXTURE_1D, entry.second);
    }

    for ( auto entry:textures.tex2d )
    {
        glActiveTexture(GL_TEXTURE0+entry.first);
        glBindTexture(GL_TEXTURE_2D, entry.second);
    }

    for ( auto entry:textures.tex3d )
    {
        glActiveTexture(GL_TEXTURE0+entry.first);
        glBindTexture(GL_TEXTURE_3D, entry.second);
    }

    for ( auto entry:constants.floats )
    {
        int location = glGetUniformLocation(program, entry.first.c_str());
        glUniform1f(location, entry.second);
    }

    for ( auto entry:constants.uints )
    {
        int location = glGetUniformLocation(program, entry.first.c_str());
        glUniform1ui(location, entry.second);
    }

    for ( auto entry:constants.fvec2s )
    {
        int location = glGetUniformLocation(program, entry.first.c_str());
        glUniform2f(location, entry.second.x, entry.second.y);
    }

    for ( auto entry:constants.fvec3s )
    {
        int location = glGetUniformLocation(program, entry.first.c_str());
        glUniform3f(location, entry.second.x, entry.second.y, entry.second.z);
    }

    for ( auto entry:constants.fvec4s )
    {
        int location = glGetUniformLocation(program, entry.first.c_str());
        glUniform4f(location, entry.second.x, entry.second.y, entry.second.z, entry.second.w);
    }
}

unsigned int RenderData::Material::ComputeHash() const
{
    unsigned int hash = 0;

    for ( auto entry:subDomainPrograms )
        hash ^= std::hash<unsigned int>{}( entry.second );

    return hash;
}

}