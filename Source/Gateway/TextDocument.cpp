#include "../Gateway/TextDocument.h"
#include "../Gateway/FileSystem.h"

#include <fstream>

namespace Gateway
{

Ref<TextDocument> TextDocument::Create()
{
	return Ref<TextDocument>(new TextDocument);
}

Ref<TextDocument> TextDocument::Load(const std::string& filepath)
{
	std::ifstream inFile(FileSystem::GetFullPath(filepath));
	if (!inFile.is_open() )
	{
		return nullptr;
	}

	Ref<TextDocument> doc = Create();

	inFile.seekg(0, inFile.end);
	int length = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	if ( length>0 )
	{
		doc->m_data.resize(length);

		inFile.read(doc->m_data.data(), length);
	}

	inFile.close();
	return doc;
}

const std::string& TextDocument::GetData() const
{
	return m_data;
}

}