#pragma once

#include "../Gateway/RefCounted.h"

#include <string>

namespace Gateway
{

class TextDocument : public RefCounted
{
public:
	static Ref<TextDocument> Create();

	static Ref<TextDocument> Load(const std::string& filepath);

public:
	const std::string& GetData() const;

protected:
	TextDocument() = default;

private:
	std::string m_data;
};

}