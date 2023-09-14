#pragma once

namespace Gateway
{

class RefCounted
{
public:
	RefCounted();

	virtual ~RefCounted();

	void Grab() const;

	void Drop() const;

	unsigned long long GetReferenceCount() const;

private:
	mutable unsigned long long m_counter;
};

template<class RefCountedType>
void Grab(RefCountedType* refCounted)
{
	if ( !refCounted )
		return;

	refCounted->Grab();
}

template<class RefCountedType>
void Drop(RefCountedType*& refCounted)
{
	if ( !refCounted )
		return;

	refCounted->Drop();

	refCounted = nullptr;
}

template<class T>
class Ref
{
public:
	Ref()
	{
		m_refCounted = nullptr;
	}

	Ref(T* obj)
	{
		m_refCounted = obj;

		Grab(m_refCounted);
	}

	Ref(const Ref<T>& obj)
	{
		m_refCounted = obj.Get();

		Grab(m_refCounted);
	}

	~Ref()
	{
		Drop(m_refCounted);
	}

	T* operator->()
	{
		return m_refCounted;
	}

	const T* operator->() const
	{
		return m_refCounted;
	}

	T& operator*()
	{
		return *m_refCounted;
	}

	T& operator[](int index)
	{
		return m_refCounted[index];
	}

	Ref<T>& operator=(const Ref<T>& obj)
	{
		m_refCounted = obj.Get();

		Grab(m_refCounted);

		return *this;
	}

	Ref<T>& operator=(T* obj)
	{
		m_refCounted = obj;

		Grab(m_refCounted);

		return *this;
	}

	bool operator==(const Ref<T>& other)
	{
		return other.m_refCounted==m_refCounted;
	}

	operator T* ( )
	{
		return m_refCounted;
	}

	T* Get() const
	{
		return m_refCounted;
	}

	template<class R>
	Ref<R> Cast() const
	{
		return Ref<R>(static_cast<R*>( m_refCounted ));
	}

	template<class R>
	operator Ref<R>() const
	{
		return Cast<R>();
	}

private:
	T* m_refCounted;
};

}