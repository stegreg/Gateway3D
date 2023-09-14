#include "../Gateway/RefCounted.h"

namespace Gateway
{

RefCounted::RefCounted()
    : m_counter(0)
{
}

RefCounted::~RefCounted()
{
}

void RefCounted::Grab() const
{
    m_counter++;
}

void RefCounted::Drop() const
{
    m_counter--;

    if ( m_counter<=0 )
    {
        delete this;
    }
}

unsigned long long RefCounted::GetReferenceCount() const
{
    return m_counter;
}

}