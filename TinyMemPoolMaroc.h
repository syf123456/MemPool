#ifndef __TINYMEMPOOLMAROC__
#define __TINYMEMPOOLMAROC__

//only support C++11
#include <assert.h>

#undef  SYF_BEGIN
#define SYF_BEGIN namespace syf{
#undef  SYF_END
#define SYF_END }

#undef US_NS_SYF
#define US_NS_SYF using namespace syf

#undef POOL_ASSERT
#define POOL_ASSERT(x) assert(x) 

#undef POOL_STATIC_ASSERT
#define POOL_STATIC_ASSERT(x) static_assert(x)

#endif
