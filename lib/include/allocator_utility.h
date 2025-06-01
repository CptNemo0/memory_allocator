#ifndef ALLOCATOR_LIB_ALLOCATOR_UTILITY_H
#define ALLOCATOR_LIB_ALLOCATOR_UTILITY_H

#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <new>
#include "aliasing.h"
#include "header.h"
#include "free_list_node.h"

namespace allocator
{
	template <typename T, typename U>
	T Rcast(U val)
	{
		return reinterpret_cast<T>(val);
	}

	inline free_list_node* AllocateHeader(const uAddress address, const std::size_t bytes)
	{
		std::cout << std::format("Allocating header at {}\n", address);
		header* header_ptr = reinterpret_cast<header*>(address);
		header_ptr->size = bytes;
		header_ptr->magic = kMagicNumber;
		return Rcast<free_list_node*>(address + sizeof(header) + bytes);
	}

	inline vAddress ToVAddress(uAddress address)
	{
		return Rcast<vAddress>(address);
	}

	inline uAddress ToUAddress(vAddress address)
	{
		return Rcast<uAddress>(address);
	}
}

#endif // !ALLOCATOR_LIB_ALLOCATOR_UTILITY_H