#include "../Gateway/Vec4.h"

namespace Gateway
{

Vec3 Vec4::XYZ() const
{
    return Vec3{ x, y, z };
}

}