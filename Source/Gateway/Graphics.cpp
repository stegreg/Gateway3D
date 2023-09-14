#include "../Gateway/Graphics.h"

#include <glad/glad.h>

#include <sstream>
#include <functional>
#include <iostream>

namespace Gateway
{

unsigned int Graphics::CompileShader(const std::string& source, unsigned int shaderType, unsigned int version, const std::vector<std::string>& defines, std::string& log)
{
    GLenum openglShaderType = 0;

    switch ( shaderType )
    {
    case SHADER_TYPE_VERTEX:
        openglShaderType = GL_VERTEX_SHADER;
        break;
    case SHADER_TYPE_FRAGMENT:
        openglShaderType = GL_FRAGMENT_SHADER;
        break;
    }

    unsigned int shaderObj = glCreateShader(openglShaderType);

    if ( !shaderObj )
        return 0;

    std::stringstream finalSourceStream;

    finalSourceStream<<"#version "<<version<<" core"<<std::endl;

    for ( const auto& define:defines )
        finalSourceStream<<"#define "<<define<<std::endl;

    finalSourceStream<<source<<std::endl;

    std::string finalSource = finalSourceStream.str();

    std::cout<<finalSource<<std::endl;

    const char* tmp = finalSource.c_str();
    glShaderSource(shaderObj, 1, &tmp, NULL);

    glCompileShader(shaderObj);

    int logLength = 0;
    glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &logLength);
    if ( logLength>0 )
    {
        log.resize(logLength);

        glGetShaderInfoLog(shaderObj, logLength, NULL, log.data());
    }

    int success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);

    if ( !success )
    {
        glDeleteShader(shaderObj);

        return 0;
    }

    return shaderObj;
}

unsigned int Graphics::LinkProgram(unsigned int vertexShaderObject, unsigned int fragmentShaderObject, std::string& log)
{
    unsigned int program = 0;

    if ( !( program = glCreateProgram() ) )
    {
        log = "Cannot create OpenGL program.\n";

        return 0;
    }

    glAttachShader(program, vertexShaderObject);
    glAttachShader(program, fragmentShaderObject);

    glLinkProgram(program);

    int linkSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if ( !linkSuccess )
    {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        if ( logLength>0 )
        {
            log.resize(logLength);
            glGetProgramInfoLog(program, logLength, NULL, log.data());
        }

        glDeleteProgram(program);

        return 0;
    }

    return program;
}

}
