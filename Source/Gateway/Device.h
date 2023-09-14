#pragma once

#include "../Gateway/RefCounted.h"

namespace Gateway
{

class Device : public RefCounted
{
public:
	static Ref<Device> Create();

	static Ref<Device> Get();

public:

private:
	Device();

	~Device();

private:
	static Device* s_singleton;
};

}