#ifndef ALLOCATOR_LIB_HEADER_H
#define ALLOCATOR_LIB_HEADER_H

#include <cstdint>
#include <cstdlib>

namespace allocator
{
	constexpr const std::uint64_t kMagicNumber = 123456789u;

	struct header
	{
		std::uint64_t magic;
		std::size_t size;
	};

	constexpr const std::uint64_t kHeaderSize = sizeof(header);
}

#endif // ! ALLOCATOR_LIB_HEADER_H
