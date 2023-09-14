#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Graphics.h"
#include "../Gateway/World.h"

#include <string>
#include <vector>

namespace Gateway
{

class ShaderSourceBase : public RefCounted
{
protected:
	static Ref<ShaderSourceBase> Load(const std::string& filepath, const std::string& shaderSourceTypeName);

public:
	void					 SetVertexSource(const std::string& source);

	void					 SetFragmentSource(const std::string& source);

	const std::string&		 GetVertexSource() const { return m_vertex; }

	const std::string&		 GetFragmentSource() const { return m_fragment; }

protected:
	ShaderSourceBase() = default;

private:
	std::string m_vertex;
	std::string m_fragment;
};

struct SubDomainSource
{
	std::string vertex;
	std::string fragment;

	std::vector<std::string> defines;
};

class ShaderDomain : public RefCounted
{
public:
	static Ref<ShaderDomain> Create();

	static Ref<ShaderDomain> Load(const std::string& filepath);

public:
	void					AddSubDomain(const std::string& name, const SubDomainSource& source);

	const SubDomainSource&	GetSubDomain(const std::string& name) const;

	const std::unordered_map<std::string, SubDomainSource>& GetSubDomains() const { return m_subDomains; }

private:
	ShaderDomain() = default;

private:
	std::unordered_map<std::string, SubDomainSource> m_subDomains;
};

class ShaderCode : public ShaderSourceBase
{
public:
	static Ref<ShaderCode> Create();

	static Ref<ShaderCode> Load(const std::string& filepath);

private:
	ShaderCode() = default;
};

class Shader : public RefCounted
{
	friend class Material;

public:
	static Ref<Shader>	Create();

	static Ref<Shader>	Load(const std::string& filepath);

public:
	void						SetCode(Ref<ShaderCode> code);

	Ref<ShaderCode>				GetCode(unsigned int type) const { return m_userCode; }

	void						SetDomain(Ref<ShaderDomain> domain);

	Ref<ShaderDomain>			GetDomain() const { return m_domainCode; }

	bool						Compile();

	RenderData::Shader			GetCompiledData(const std::string& subDomainName) const;

	const std::unordered_map<std::string, RenderData::Shader>& GetCompiledSubDomains() const { return m_compiledSubDomains; }
private:
	Shader() = default;

	~Shader() = default;

	//void ExtractShaderData(RenderData::Shader& renderShader);

private:
	Ref<ShaderCode>		m_userCode;

	Ref<ShaderDomain>	m_domainCode;

	std::unordered_map<std::string, RenderData::Shader> m_compiledSubDomains;
};

}