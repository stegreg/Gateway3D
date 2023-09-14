#pragma once

#include "../Gateway/RefCounted.h"

#include <string>
#include <vector>

namespace Gateway
{

enum
{
	TEXTURE_FORMAT_R8,
	TEXTURE_FORMAT_RGB8,
	TEXTURE_FORMAT_RGBA8,
	TEXTURE_FORMAT_RGBA16_FLOAT,
	TEXTURE_FORMAT_RGBA32_FLOAT,
	TEXTURE_FORMAT_DEPTH
};

enum
{
	TEXTURE_FLAG_NONE	= 0,
	TEXTURE_FLAG_MANUAL	= 1 << 0,
};

class Texture : public RefCounted
{
	friend class RenderWindow;
	friend class RenderPath;

public:
	static Ref<Texture> Create(unsigned int width, unsigned int height, unsigned int format = TEXTURE_FORMAT_RGBA8, unsigned int flags = TEXTURE_FLAG_NONE);

	static bool			Import(const std::string& filepath);

	static bool			Save(Ref<Texture> texture, const std::string& filepath);

	static Ref<Texture> Load(const std::string& filepath, unsigned int flags = TEXTURE_FLAG_NONE);

	static unsigned int GetComponentsByteSize(unsigned int format);

public:	
	void			SetPixels(const unsigned char* pixels);

	void			SetPixels(const std::vector<unsigned char>& pixels);

	void			CopyPixels(std::vector<unsigned char>& destination) const;

	void			FlipY();

	void			SetRepeat(bool u, bool v);

	void			SetSmooth(bool smooth);

	unsigned int	GetByteSize() const;

	unsigned int	GetWidth() const;

	unsigned int	GetHeight() const;

	unsigned int	GetTextureObject() const { return m_textureObject; }

	void			TransitionToDeviceMemory();

	void			FreeDeviceMemory();

	const std::string& GetFilePath() const { return m_filePath; }

protected:
	Texture(unsigned int width, unsigned int height, unsigned int format, unsigned int flags);
	
	Texture(unsigned int glObject);

	~Texture();

private:
	std::string m_filePath;

	unsigned int m_textureObject = 0;

	unsigned int m_width = 0, m_height = 0;

	unsigned int m_format = 0;

	unsigned int m_flags = 0;

	bool		 m_repeatU = 0, m_repeatV = 0;

	bool		 m_smooth = true;

	std::vector<unsigned char> m_data;

	bool		m_dataDirty = false;
};

}