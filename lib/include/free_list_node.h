#ifndef ALLOCATOR_LIB_FREE_LIST_NODE_H
#define ALLOCATOR_LIB_FREE_LIST_NODE_H

#include <cstdint>
#include <cstdlib>

namespace allocator
{
	struct free_list_node
	{
		free_list_node* next;	// Pointer to the next node in the free list
		std::size_t size;		// Size of the block of memory this node represents
	};

	constexpr const std::uint64_t kFreeListNodeSize = sizeof(free_list_node);
}

#endif // ! ALLOCATOR_LIB_FREE_LIST_NODE_H
