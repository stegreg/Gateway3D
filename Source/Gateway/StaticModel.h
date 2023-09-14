#pragma once

#include "../Gateway/Resource.h"

#include <vector>

namespace Gateway
{

class StaticModel : public Resource
{
public:
	enum AttributeType
	{
		ATTRIBUTE_POSITION	= 1 << 0,
		ATTRIBUTE_NORMAL	= 1 << 1,
		ATTRIBUTE_UV0		= 1 << 2,
	};

public:
	StaticModel& BeginBuild()
	{
		vertexBuffer_.clear();
		indexBuffer_.clear();

		return *this;
	}

	StaticModel& AddPosition(float x, float y, float z)
	{
		vertexBuffer_.resize(vertexBuffer_.size()+3);

		vertexBuffer_.push_back(x);
		vertexBuffer_.push_back(y);
		vertexBuffer_.push_back(z);

		attributesLayout_ = attributesLayout_ | ATTRIBUTE_POSITION;

		return *this;
	}

	StaticModel& AddIndex(const unsigned int index)
	{
		indexBuffer_.push_back(index);

		return *this;
	}

	StaticModel& EndBuild()
	{
		//TODO: compute aabb, etc...
		return *this;
	}

private:
	virtual void Load(std::stringstream& inStream, Residency toResidency) override;
	
	virtual void Unload(Residency fromResidency) override;
	
	virtual void Save(std::stringstream& outStream) override;

private:
	std::vector<float> vertexBuffer_;
	std::vector<unsigned int> indexBuffer_;

	int attributesLayout_ = 0;
};

}