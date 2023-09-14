#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Vec2.h"
#include "../Gateway/Mat4.h"

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

namespace Gateway
{

class RmlSystemImpl : public Rml::SystemInterface, public RefCounted
{
public:
	double GetElapsedTime() override;
};

struct RmlGeometry
{
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	unsigned int numIndices;

	unsigned int texture;
};

class RmlRenderImpl : public Rml::RenderInterface
{
public:
	void			SetCurrentContext(Rml::Context* context);

	void			RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture,
						const Rml::Vector2f& translation) override;

	Rml::CompiledGeometryHandle 
					CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture);

	void			RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation);

	void			ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry);

	void			EnableScissorRegion(bool enable) override;

	void			SetScissorRegion(int x, int y, int width, int height) override;

	bool			LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

	virtual bool	GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;

	virtual void	ReleaseTexture(Rml::TextureHandle texture) override;

	void			UpdateContextData();

	static RmlRenderImpl* Get()
	{
		static RmlRenderImpl rmlRenderImpl;

		return &rmlRenderImpl;
	}

private:
	RmlRenderImpl();

	~RmlRenderImpl();

private:
	struct ContextData
	{
		unsigned int vertexShader = 0;
		unsigned int fragmentShader_Color = 0;
		unsigned int fragmentShader_Texture = 0;

		unsigned int programColor = 0;
		unsigned int programTexture = 0;
	};

	std::unordered_map<unsigned int, ContextData> m_contextData;

	Rml::Context*	m_currentContext = nullptr;
};

class UI : public RefCounted
{
public:
	static Ref<UI>			  Create();

	static Ref<RmlSystemImpl> GetSystemImpl();

private:
	UI();
	
	~UI();

private:
	static Ref<RmlSystemImpl> s_rmlSystemImpl;
};

}