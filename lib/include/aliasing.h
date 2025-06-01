#ifndef ALLOCATOR_LIB_ALIASING_H
#define ALLOCATOR_LIB_ALIASING_H

#include <cstdint>

namespace allocator
{
	using vAddress = void*;
	using uAddress = std::uintptr_t;
}

#endif // !ALLOCATOR_LIB_ALIASING_H