#ifndef __TINYCONSTRUCTHELP_H__
#define __TINYCONSTRUCTHELP_H__
#include "TinyMemPoolMaroc.h"
SYF_BEGIN
template<typename T>
class TinyConstructHelp
{
public:
	typedef T value_type;
public:
	~TinyConstructHelp()
	{
	}

	static value_type* construct(void *dst)
	{
		POOL_ASSERT(dst);
		return ::new(dst) value_type();
	}

	template<typename ...Args>
	static value_type* construct(void *dst, Args&... arg)
	{
		POOL_ASSERT(dst);
		return ::new(dst) value_type(arg...);
	}

	template<typename ...Args>
	static value_type* construct(void *dst, Args&&... arg)
	{
		POOL_ASSERT(dst);
		return ::new(dst) value_type(arg...);
	}

	static value_type* construct(void *dst, const value_type &v)
	{
		POOL_ASSERT(dst);
		return ::new(dst) value_type(v);
	}

	static value_type* construct(void *dst, const value_type &&v)
	{
		POOL_ASSERT(dst);
		return ::new(dst) value_type(v);
	}

	static void destroy(value_type *dst)
	{
		POOL_ASSERT(dst);
		dst->~value_type();
	}
protected:
	TinyConstructHelp() {}
	TinyConstructHelp(const TinyConstructHelp &t)=delete;
	TinyConstructHelp(const TinyConstructHelp &&t)=delete;
	TinyConstructHelp& operator=(const TinyConstructHelp &t) = delete;
	TinyConstructHelp& operator=(const TinyConstructHelp &&t) = delete;
};
SYF_END

#endif

