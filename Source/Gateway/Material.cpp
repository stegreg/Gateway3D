#include "../Gateway/Material.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/Context.h"

#include <glad/glad.h>

#include <nlohmann/json.hpp>

#include <toml.hpp>

#include <sstream>
#include <fstream>
#include <iostream>

namespace Gateway
{
Ref<Material> Material::Create()
{
    return Ref<Material>(new Material);
}

Ref<Material> Material::Load(const std::string& filepath)
{
    static std::unordered_map<unsigned int, std::unordered_map<std::string, Ref<Material>>> s_contextCache;

    unsigned int currentContextID = Context::GetCurrentID();

    if ( s_contextCache[currentContextID].contains(filepath) )
        return s_contextCache[currentContextID].at(filepath);

    Ref<Material> material = Material::Create();

    toml::value data;
    toml::value materialValue;
    try
    {
        data = toml::parse(FileSystem::GetFullPath(filepath));

        materialValue = toml::find(data, "material");
    }
    catch ( std::out_of_range& ex )
    {
        return nullptr;
    }

    {
        std::string shaderPath = toml::find_or<std::string>(materialValue, "shader-path", "");

        Ref<Shader> shaderCode = Shader::Load(shaderPath);

        if ( !shaderCode )
        {
            return nullptr;
        }

        material->SetShader(shaderCode);

        if ( !material->Link() )
        {
            
        }
    }

    if ( materialValue.contains("textures") )
    {
        toml::array textures = toml::find(materialValue, "textures").as_array();

        for ( auto textureEntry:textures )
        {
            std::string  name        = toml::find<std::string>(textureEntry, "name");
            std::string  texturePath = toml::find<std::string>(textureEntry, "path");
            unsigned int slot        = toml::find<unsigned int>(textureEntry, "slot");

            Ref<Texture> texture = Texture::Load(texturePath);

            material->SetTexture(name, texture, slot);
        }
    }

    if ( materialValue.contains("constants") )
    {
        toml::array constants = toml::find(materialValue, "constants").as_array();

        for ( auto constantsEntry:constants )
        {
            std::string  name = toml::find<std::string>(constantsEntry, "name");
            toml::value  value = toml::find(constantsEntry, "value");

            if ( value.is_floating() )
            {
                float fValue = value.as_floating();

                material->SetConstant(name, fValue);
            }

            if ( value.is_array() )
            {
                toml::array aValue = value.as_array();

                if ( aValue.size()==2 )
                {
                    fVec2 fVecValue = { aValue.at(0).as_floating(), aValue.at(1).as_floating() };

                    material->SetConstant(name, fVecValue);
                }

                if ( aValue.size()==3 )
                {
                    Vec3 fVecValue = { aValue.at(0).as_floating(), aValue.at(1).as_floating(), aValue.at(2).as_floating() };

                    material->SetConstant(name, fVecValue);
                }
            }
        }
    }

    material->m_filepath = filepath;

    s_contextCache[currentContextID][filepath] = material;
    return material;
}

Material::Material()
{

}

Material::~Material()
{

}

void Material::ExtractRenderMaterial(RenderData::Material& renderMaterial)
{
    renderMaterial = m_renderMaterial;
}

void Material::SetShader(Ref<Shader> shader)
{
    m_shader = shader;
}

Ref<Shader> Material::GetShader()
{
    return m_shader;
}

bool Material::Link()
{
    std::string log;

    const std::unordered_map<std::string, RenderData::Shader>& compiledSubDomains = m_shader->GetCompiledSubDomains();

    for ( auto entry:compiledSubDomains )
    {
        const RenderData::Shader& shader = entry.second;

        std::string log;

        if ( !( m_renderMaterial.subDomainPrograms[entry.first] = Graphics::LinkProgram(shader.vertexShaderID, shader.fragmentShaderID, log) ) )
        {
            std::cout<<"Error while linking subdomain "<<entry.first<<": "<<log<<std::endl;

            return false;
        }
    }

    return true;
}

void Material::SetTexture(const std::string& name, Ref<Texture> param, unsigned int slot)
{
    m_renderMaterial.textures.tex2d[slot] = param->GetTextureObject();
}

void Material::SetConstant(const std::string& name, float param)
{
    m_renderMaterial.constants.floats[name] = param;
}

void Material::SetConstant(const std::string& name, const fVec2& param)
{
    m_renderMaterial.constants.fvec2s[name] = param;
}

void Material::SetConstant(const std::string& name, const Vec3& param)
{
    m_renderMaterial.constants.fvec3s[name] = param;
}

}