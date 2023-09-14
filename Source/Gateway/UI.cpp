#include "../Gateway/UI.h"
#include "../Gateway/Texture.h"
#include "../Gateway/Graphics.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/Context.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>

namespace Gateway
{

double RmlSystemImpl::GetElapsedTime()
{
	static const Uint64 start = SDL_GetPerformanceCounter();
	static const double frequency = double(SDL_GetPerformanceFrequency());
	return double(SDL_GetPerformanceCounter()-start)/frequency;
}

RmlRenderImpl::RmlRenderImpl()
{
    UpdateContextData();
}

RmlRenderImpl::~RmlRenderImpl()
{
}

void RmlRenderImpl::SetCurrentContext(Rml::Context* context)
{
    m_currentContext = context;
}

void RmlRenderImpl::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation)
{
    Rml::CompiledGeometryHandle geometry = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);

    if ( geometry )
    {
        RenderCompiledGeometry(geometry, translation);
        ReleaseCompiledGeometry(geometry);
    }
}

Rml::CompiledGeometryHandle RmlRenderImpl::CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle textureHandle)
{
    RmlGeometry* geom = new RmlGeometry;

    glGenBuffers(1, &geom->vbo);
    glGenBuffers(1, &geom->ebo);
    glGenVertexArrays(1, &geom->vao);

    glBindVertexArray(geom->vao);

    glBindBuffer(GL_ARRAY_BUFFER, geom->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Rml::Vertex)*num_vertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*num_indices, indices, GL_STATIC_DRAW);

    unsigned int stride = sizeof(Rml::Vertex);
    unsigned int offset = offsetof(Rml::Vertex, position);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)( offset ));

    glEnableVertexAttribArray(1);
    offset = offsetof(Rml::Vertex, colour);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)( offset ));

    glEnableVertexAttribArray(2);
    offset = offsetof(Rml::Vertex, tex_coord);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)( offset ));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if ( textureHandle )
    {
        Texture* texture = (Texture*)textureHandle;

        geom->texture = texture->GetTextureObject();
    }
    else
    {
        geom->texture = 0;
    }

    geom->numIndices = num_indices;

    return (Rml::CompiledGeometryHandle)geom;
}

void RmlRenderImpl::RenderCompiledGeometry(Rml::CompiledGeometryHandle handle, const Rml::Vector2f& translation)
{
    unsigned int contextID = Context::GetCurrentID();
    RmlGeometry* geom = (RmlGeometry*)handle;

    unsigned int program = 0;

    if ( geom->texture )
    {
        program = m_contextData[contextID].programTexture;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, geom->texture);
    }
    else
    {
        program = m_contextData[contextID].programColor;
    }

    glUseProgram(program);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(geom->vao);

    Rml::Vector2i dimensions = m_currentContext->GetDimensions();
    Rml::Matrix4f projection = Rml::Matrix4f::ProjectOrtho(0, dimensions.x, dimensions.y, 0, -10000, 10000);

    int translation_location = glGetUniformLocation(program, "_translate");
    glUniform2f(translation_location, translation.x, translation.y);

    int transform_location = glGetUniformLocation(program, "_transform");
    glUniformMatrix4fv(transform_location, 1, GL_FALSE, (const GLfloat*)projection.data());

    int texture_location = glGetUniformLocation(program, "_tex");
    glUniform1i(texture_location, 0);

    glDrawElements(GL_TRIANGLES, geom->numIndices, GL_UNSIGNED_INT, (void*)0);

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
}

void RmlRenderImpl::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle handle)
{
    RmlGeometry* geom = (RmlGeometry*)handle;

    glDeleteBuffers(1, &geom->vbo);
    glDeleteBuffers(1, &geom->ebo);
    glDeleteVertexArrays(1, &geom->vao);

    delete geom;
}

void RmlRenderImpl::EnableScissorRegion(bool enable)
{
    if ( enable )
    {
        glEnable(GL_SCISSOR_TEST);
    }
    else
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void RmlRenderImpl::SetScissorRegion(int x, int y, int width, int height)
{
    Rml::Vector2i dimensions = m_currentContext->GetDimensions();
    glScissor(x, dimensions.y-( y+height ), width, height);
}

bool RmlRenderImpl::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
    Ref<Texture> texture = Texture::Load(source);

    texture->FlipY();

    texture_dimensions.x = texture->GetWidth();
    texture_dimensions.y = texture->GetHeight();

    texture->Grab();
    texture_handle = (Rml::TextureHandle)texture.Get();

    return true;
}

bool RmlRenderImpl::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions)
{
    Ref<Texture> texture = Texture::Create(source_dimensions.x, source_dimensions.y, TEXTURE_FORMAT_RGBA8);

    texture->SetPixels(source);

    texture->Grab();
    texture_handle = (Rml::TextureHandle)texture.Get();

    return true;
}

void RmlRenderImpl::ReleaseTexture(Rml::TextureHandle textureHandle)
{
    Texture* texture = (Texture*)textureHandle;

    texture->Drop();
}

void RmlRenderImpl::UpdateContextData()
{
    unsigned int contextID = Context::GetCurrentID();

    if ( m_contextData.contains(contextID) )
        return;

    std::string vertexShaderSource =
        "uniform vec2 _translate;\n"
        "uniform mat4 _transform;\n"

        "layout(location = 0) in vec2 inPosition;\n"
        "layout(location = 1) in vec4 inColor0;\n"
        "layout(location = 2) in vec2 inTexCoord0;\n"

        "out vec2 fragTexCoord;\n"
        "out vec4 fragColor;\n"

        "void main()\n"
        "{\n"
        "	fragTexCoord = inTexCoord0;\n"
        "	fragColor = inColor0;\n"

        "	vec2 translatedPos = inPosition+_translate.xy;\n"
        "	vec4 outPos = _transform*vec4(translatedPos, 0, 1);\n"

        "	gl_Position = outPos;\n"
        "}\n";

    std::string fragmentShaderSource_Color =
        "in vec2 fragTexCoord;\n"
        "in vec4 fragColor;\n"

        "out vec4 finalColor;\n"

        "void main()\n"
        "{\n"
        "	finalColor = fragColor;\n"
        "}\n";

    std::string fragmentShaderSource_Texture =
        "uniform sampler2D _tex;\n"
        "in vec2 fragTexCoord;\n"
        "in vec4 fragColor;\n"

        "out vec4 finalColor;\n"

        "void main()\n"
        "{\n"
        "	vec4 texColor = texture(_tex, fragTexCoord);\n"
        "	finalColor = fragColor*texColor;\n"
        "}\n";

    std::string log;

    m_contextData[contextID].vertexShader = Graphics::CompileShader(vertexShaderSource, SHADER_TYPE_VERTEX, 330, {}, log);

    m_contextData[contextID].fragmentShader_Color = Graphics::CompileShader(fragmentShaderSource_Color, SHADER_TYPE_FRAGMENT, 330, {}, log);
    m_contextData[contextID].fragmentShader_Texture = Graphics::CompileShader(fragmentShaderSource_Texture, SHADER_TYPE_FRAGMENT, 330, {}, log);

    m_contextData[contextID].programColor = Graphics::LinkProgram(m_contextData[contextID].vertexShader, m_contextData[contextID].fragmentShader_Color, log);
    m_contextData[contextID].programTexture = Graphics::LinkProgram(m_contextData[contextID].vertexShader, m_contextData[contextID].fragmentShader_Texture, log);
}

Ref<RmlSystemImpl> UI::s_rmlSystemImpl = nullptr;

UI::UI()
{
    s_rmlSystemImpl = new RmlSystemImpl;

    Rml::SetSystemInterface(s_rmlSystemImpl.Get());

    Rml::SetRenderInterface(RmlRenderImpl::Get());

    Rml::Initialise();

    Rml::LoadFontFace(FileSystem::GetFullPath("UI/LatoLatin-Bold.ttf"));
}

UI::~UI()
{
    Rml::Shutdown();
}

Ref<UI> UI::Create()
{
    return Ref<UI>(new UI);
}

Ref<RmlSystemImpl> UI::GetSystemImpl()
{
    return s_rmlSystemImpl;
}

}