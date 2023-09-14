#include "../Gateway/Texture.h"
#include "../Gateway/Window.h"
#include "../Gateway/Context.h"
#include "../Gateway/FileSystem.h"

#include <glad/glad.h>

#include <toml.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>

#include <fstream>
#include <unordered_map>

namespace Gateway
{

Ref<Texture> Texture::Create(unsigned int width, unsigned int height, unsigned int format, unsigned int flags)
{
	return Ref<Texture>(new Texture(width, height, format, flags));
}

bool Texture::Import(const std::string& filepath)
{
	stbi_set_flip_vertically_on_load(true);

	int w, h, c;
	unsigned char* data = stbi_load(FileSystem::GetFullPath(filepath).c_str(), &w, &h, &c, 0);
	
	if ( !data )
		return false;

	unsigned int format = 0;

	switch ( c )
	{
	case 1:
		format = TEXTURE_FORMAT_R8;
		break;
	case 3:
		format = TEXTURE_FORMAT_RGB8;
		break;
	case 4:
		format = TEXTURE_FORMAT_RGBA8;
		break;
	}

	Ref<Texture> texture = Create(w, h, format, TEXTURE_FLAG_MANUAL);

	texture->SetPixels(data);

	texture->Save(texture, filepath);

	return true;
}

bool Texture::Save(Ref<Texture> texture, const std::string& filepath)
{
	std::string importFileName	= filepath+".import";

	std::ofstream outFile(FileSystem::GetFullPath(importFileName), std::ios::binary);

	if ( !outFile.is_open() )
		return false;

	unsigned int width	= texture->m_width;
	unsigned int height = texture->m_height;
	unsigned int format = texture->m_format;

	unsigned int byteSize = texture->GetByteSize();

	unsigned char* data = texture->m_data.data();

	outFile.write((const char*)&width, sizeof(unsigned int));
	outFile.write((const char*)&height, sizeof(unsigned int));
	outFile.write((const char*)&format, sizeof(unsigned int));
	outFile.write((const char*)&byteSize, sizeof(unsigned int));

	outFile.write((const char*)data, byteSize);

	outFile.close();
	return true;
}

Ref<Texture> Texture::Load(const std::string& filepath, unsigned int flags)
{
	static std::unordered_map<unsigned int, std::unordered_map<std::string, Ref<Texture>>> s_contextCache;

	unsigned int currentContextID = Context::GetCurrentID();

	if ( s_contextCache[currentContextID].contains(filepath) )
		return s_contextCache[currentContextID].at(filepath);

	toml::value metaData	= toml::parse(FileSystem::GetFullPath(filepath));
	toml::value textureData = toml::find(metaData, "texture");

	toml::value importData	= toml::find(textureData, "import");

	std::vector<bool>	repeatComponents = toml::get<std::vector<bool>>(toml::find(textureData, "repeat"));
	bool				smooth			 = toml::find<bool>(textureData, "smooth");
	std::string			dataPath		 = toml::find<std::string>(importData, "path");

	std::ifstream inFile(FileSystem::GetFullPath(dataPath), std::ios::binary);

	if ( !inFile.is_open() )
		return nullptr;

	std::vector<unsigned char> pixels;
	unsigned int width, height, format, byteSize;

	inFile.read((char*)&width, sizeof(unsigned int));
	inFile.read((char*)&height, sizeof(unsigned int));
	inFile.read((char*)&format, sizeof(unsigned int));
	inFile.read((char*)&byteSize, sizeof(unsigned int));

	pixels.resize(byteSize);
	inFile.read((char*)pixels.data(), byteSize);

	Ref<Texture> texture = Create(width, height, format, flags);
	texture->SetPixels(pixels);
	texture->SetRepeat(repeatComponents.at(0), repeatComponents.at(1));
	texture->SetSmooth(smooth);

	s_contextCache[currentContextID][filepath] = texture;

	texture->m_filePath = filepath;

	return texture;
}

unsigned int Texture::GetComponentsByteSize(unsigned int format)
{
	switch ( format )
	{
	case TEXTURE_FORMAT_R8:
		return sizeof(unsigned char);
		break;
	case TEXTURE_FORMAT_RGB8:
		return sizeof(unsigned char)*3;
		break;
	case TEXTURE_FORMAT_RGBA8:
		return sizeof(unsigned char)*4;
		break;
	case TEXTURE_FORMAT_RGBA16_FLOAT:
		return 16*4;
		break;
	case TEXTURE_FORMAT_RGBA32_FLOAT:
		return sizeof(float)*4;
		break;
	}
}

void Texture::SetPixels(const unsigned char* pixels)
{
	if ( pixels )
	{
		unsigned int byteSize = GetByteSize();

		m_data.resize(byteSize);
		memcpy(m_data.data(), pixels, byteSize);
	}

	m_dataDirty = true;

	if ( !( m_flags&TEXTURE_FLAG_MANUAL ) )
		TransitionToDeviceMemory();
}

void Texture::SetPixels(const std::vector<unsigned char>& pixels)
{
	SetPixels(pixels.data());
}

void Texture::CopyPixels(std::vector<unsigned char>& destination) const
{
	if ( m_data.empty() )
		return;

	destination.resize(GetByteSize());

	memcpy(destination.data(), m_data.data(), GetByteSize());
}

void Texture::FlipY()
{
	if ( m_data.empty() )
		return;

	for ( unsigned int y = 0; y<m_height/2; y++ )
	{
		unsigned int rowByteSize = m_width*GetComponentsByteSize(m_format);
		
		for ( unsigned int b = 0; b<rowByteSize; b++ )
		{
			unsigned int pos0 = y*rowByteSize+b;
			unsigned int pos1 = (m_height-1)*rowByteSize+b-( y*rowByteSize );

			std::swap(m_data.at(pos0), m_data.at(pos1));
		}
	}

	m_dataDirty = true;

	if ( !( m_flags&TEXTURE_FLAG_MANUAL ) )
		TransitionToDeviceMemory();
}

void Texture::SetRepeat(bool repeatU, bool repeatV)
{
	m_repeatU = repeatU;
	m_repeatV = repeatV;

	if ( !( m_flags&TEXTURE_FLAG_MANUAL ) )
		TransitionToDeviceMemory();
}

void Texture::SetSmooth(bool smooth)
{
	m_smooth = smooth;

	if ( !( m_flags&TEXTURE_FLAG_MANUAL ) )
		TransitionToDeviceMemory();
}

unsigned int Texture::GetByteSize() const
{
	return GetComponentsByteSize(m_format)*GetWidth()*GetHeight();;
}

unsigned int Texture::GetWidth() const
{
	return m_width;
}

unsigned int Texture::GetHeight() const
{
	return m_height;
}

void Texture::TransitionToDeviceMemory()
{
	if ( !m_textureObject )
		glGenTextures(1, &m_textureObject);

	glBindTexture(GL_TEXTURE_2D, m_textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_repeatU ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_repeatV ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	if ( m_dataDirty )
	{
		unsigned int internalFormat = 0;
		unsigned int format = 0;
		unsigned int componentType = 0;

		switch ( m_format )
		{
		case TEXTURE_FORMAT_R8:
			format = GL_RED;
			internalFormat = GL_R8;
			componentType = GL_UNSIGNED_BYTE;
			break;
		case TEXTURE_FORMAT_RGB8:
			format = GL_RGB;
			internalFormat = GL_RGB8;
			componentType = GL_UNSIGNED_BYTE;
			break;
		case TEXTURE_FORMAT_RGBA8:
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
			componentType = GL_UNSIGNED_BYTE;
			break;
		case TEXTURE_FORMAT_RGBA16_FLOAT:
			format = GL_RGBA;
			internalFormat = GL_RGBA16F;
			componentType = GL_FLOAT;
			break;
		case TEXTURE_FORMAT_RGBA32_FLOAT:
			format = GL_RGBA;
			internalFormat = GL_RGBA32F;
			componentType = GL_FLOAT;
			break;
		case TEXTURE_FORMAT_DEPTH:
			format = GL_DEPTH_COMPONENT;
			internalFormat = GL_DEPTH_COMPONENT;
			componentType = GL_FLOAT;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, m_textureObject);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, componentType, m_data.data());
		
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 12.0);
		
		glBindTexture(GL_TEXTURE_2D, 0);

		m_dataDirty = false;
	}
}

void Texture::FreeDeviceMemory()
{
	glDeleteTextures(1, &m_textureObject);
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int format, unsigned int flags)
	: m_width(width), m_height(height), m_format(format), m_flags(flags)
{
}

Texture::Texture(unsigned int glObject)
{
	m_textureObject = glObject;

	m_flags = TEXTURE_FLAG_MANUAL;

	//TODO: init properties from gl object...
}

Texture::~Texture()
{
	if ( !( m_flags&TEXTURE_FLAG_MANUAL ) )
		FreeDeviceMemory();
}

}