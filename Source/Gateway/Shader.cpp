#include "../Gateway/Shader.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/TextDocument.h"
#include "../Gateway/Context.h"

#include <glad/glad.h>
#include <nlohmann/json.hpp>

#include <toml.hpp>

#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#define VS_DEFAULT_PATH "Shaders/GLSL/Default.vert"
#define FS_DEFAULT_PATH "Shaders/GLSL/Default.frag"

namespace Gateway
{

Ref<Shader> Shader::Create()
{
    return Ref<Shader>(new Shader);
}

Ref<Shader> Shader::Load(const std::string& filepath)
{
    static std::unordered_map<unsigned int, std::unordered_map<std::string, Ref<Shader>>> s_contextCache;

    unsigned int currentContextID = Context::GetCurrentID();

    if ( s_contextCache[currentContextID].contains(filepath) )
        return s_contextCache[currentContextID].at(filepath);

    Ref<Shader> shader = Create();

    toml::value data;
    toml::value shaderValue;
    try
    {
        data        = toml::parse(FileSystem::GetFullPath(filepath));

        shaderValue = toml::find(data, "shader");
    }
    catch ( std::out_of_range& ex )
    {
        return nullptr;
    }

    std::string domainPath = toml::find_or<std::string>(shaderValue, "domain-path", "");
    std::string codePath = toml::find_or<std::string>(shaderValue, "code-path", "");

    std::vector<std::string> defines = toml::find_or<std::vector<std::string>>(shaderValue, "defines", std::vector<std::string>());

    Ref<ShaderDomain> domain = ShaderDomain::Load(domainPath);
    Ref<ShaderCode>   code   = ShaderCode::Load(codePath);

    if ( !domain||!code )
    {
        return nullptr;
    }

    shader->SetDomain(domain);
    shader->SetCode(code);
    //shader->SetDefines(defines);

    //if ( !shader->Compile() )
    {

    }

    shader->Compile();

    s_contextCache[currentContextID][filepath] = shader;
    return shader;
}

void Shader::SetCode(Ref<ShaderCode> code)
{
    m_userCode = code;
}

void Shader::SetDomain(Ref<ShaderDomain> domain)
{
    m_domainCode = domain;
}

bool Shader::Compile()
{
    const std::unordered_map<std::string, SubDomainSource>& subDomains = m_domainCode->GetSubDomains();

    for ( const auto& entry:subDomains )
    {
        std::string replaceString = "#pragma __USER_CODE__";
        size_t      replacePoint = 0;

        std::string finalVertexShader = entry.second.vertex;
        std::string vsUser = m_userCode->GetVertexSource();
        replacePoint = finalVertexShader.find(replaceString);
        if ( replacePoint!=std::string::npos )
            finalVertexShader.replace(replacePoint, replaceString.size(), vsUser.c_str());

        std::string finalFragmentShader = entry.second.fragment;
        std::string fsUser = m_userCode->GetFragmentSource();
        replacePoint = finalFragmentShader.find(replaceString);
        if ( replacePoint!=std::string::npos )
            finalFragmentShader.replace(replacePoint, replaceString.size(), fsUser.c_str());

        std::string log;
        m_compiledSubDomains[entry.first].vertexShaderID = Graphics::CompileShader(finalVertexShader, SHADER_TYPE_VERTEX, 430, entry.second.defines, log);

        std::cout<<log<<std::endl;

        m_compiledSubDomains[entry.first].fragmentShaderID = Graphics::CompileShader(finalFragmentShader, SHADER_TYPE_FRAGMENT, 430, entry.second.defines, log);

        std::cout<<log<<std::endl;
    }

    return true;
}

RenderData::Shader Shader::GetCompiledData(const std::string& subDomainName) const
{
    if ( !m_compiledSubDomains.contains(subDomainName) )
        return RenderData::Shader();

    return m_compiledSubDomains.at(subDomainName);
}

//
//void Shader::ExtractShaderData(RenderData::Shader& renderShader)
//{
//
//    renderShader = m_renderShader;
//}

Ref<ShaderDomain> ShaderDomain::Create()
{
    return new ShaderDomain;
}

Ref<ShaderDomain> ShaderDomain::Load(const std::string& filepath)
{
    static std::unordered_map<unsigned int, std::unordered_map<std::string, Ref<ShaderDomain>>> s_contextCache;

    unsigned int currentContextID = Context::GetCurrentID();

    if ( s_contextCache[currentContextID].contains(filepath) )
        return s_contextCache[currentContextID].at(filepath);

    Ref<ShaderDomain> domain = Create();

    toml::value data;
    toml::value shaderDomainValue;
    toml::array subDomainsArray;
    try
    {
        data = toml::parse(FileSystem::GetFullPath(filepath));

        shaderDomainValue = toml::find(data, "shader-domain");

        subDomainsArray = toml::find(shaderDomainValue, "sub-domains").as_array();
    }
    catch ( std::out_of_range& ex )
    {
        return nullptr;
    }

    for ( auto subDomainValue:subDomainsArray )
    {
        std::string name                 = toml::find<std::string>(subDomainValue, "name");
        std::string vsPath               = toml::find<std::string>(subDomainValue, "vertex-source-path");
        std::string fsPath               = toml::find<std::string>(subDomainValue, "fragment-source-path");
        std::vector<std::string> defines = toml::find_or<std::vector<std::string>>(subDomainValue, "defines", std::vector<std::string>());

        Ref<TextDocument> vsDocument = TextDocument::Load(vsPath);
        Ref<TextDocument> fsDocument = TextDocument::Load(fsPath);

        if ( !vsDocument||!fsDocument )
            return nullptr;

        std::string vsSource = vsDocument->GetData();
        std::string fsSource = fsDocument->GetData();

        SubDomainSource subDomainSource;
        subDomainSource.vertex   = vsSource;
        subDomainSource.fragment = fsSource;
        subDomainSource.defines  = defines;

        domain->AddSubDomain(name, subDomainSource);
    }

    s_contextCache[currentContextID][filepath] = domain;

    return domain;
}

void ShaderDomain::AddSubDomain(const std::string& name, const SubDomainSource& source)
{
    m_subDomains[name] = source;
}

const SubDomainSource& ShaderDomain::GetSubDomain(const std::string& name) const
{
    if ( !m_subDomains.contains(name) )
        return SubDomainSource();

    return m_subDomains.at(name);
}

Ref<ShaderCode> ShaderCode::Create()
{
    return new ShaderCode;
}

Ref<ShaderCode> ShaderCode::Load(const std::string& filepath)
{
    return ShaderSourceBase::Load(filepath, "shader-code");
}

Ref<ShaderSourceBase> ShaderSourceBase::Load(const std::string& filepath, const std::string& shaderSourceTypeName)
{
    static std::unordered_map<unsigned int, std::unordered_map<std::string, Ref<ShaderSourceBase>>> s_contextCache;

    unsigned int currentContextID = Context::GetCurrentID();

    if ( s_contextCache[currentContextID].contains(filepath) )
        return s_contextCache[currentContextID].at(filepath);

    Ref<ShaderSourceBase> code = ShaderCode::Create();

    toml::value data;
    toml::value shaderValue;
    try
    {
        data = toml::parse(FileSystem::GetFullPath(filepath));

        shaderValue = toml::find(data, shaderSourceTypeName);
    }
    catch ( std::out_of_range& ex )
    {
        return nullptr;
    }

    std::string vsPath = toml::find_or<std::string>(shaderValue, "vertex-source-path", "");
    std::string fsPath = toml::find_or<std::string>(shaderValue, "fragment-source-path", "");

    Ref<TextDocument> vsDocument = TextDocument::Load(vsPath);
    Ref<TextDocument> fsDocument = TextDocument::Load(fsPath);

    if ( !vsDocument||!fsDocument )
        return nullptr;

    std::string vsSource = vsDocument->GetData();
    std::string fsSource = fsDocument->GetData();

    code->SetVertexSource(vsSource);
    code->SetFragmentSource(fsSource);

    return code;
}

void ShaderSourceBase::SetVertexSource(const std::string& source)
{
    m_vertex = source;
}

void ShaderSourceBase::SetFragmentSource(const std::string& source)
{
    m_fragment = source;
}

}
