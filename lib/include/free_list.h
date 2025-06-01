#ifndef ALLOCATOR_LIB_FREE_LIST_H
#define ALLOCATOR_LIB_FREE_LIST_H

#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <utility>

#include "aliasing.h"
#include "free_list_node.h"
#include "header.h"

namespace allocator
{
	class free_list
	{
	private:
		std::uint64_t capacity_;		// Total capacity of the free list
		free_list_node* head_;			// Pointer to the head of the free list
		vAddress data_ = nullptr;
	
		std::pair<free_list_node*, free_list_node*> FindSuitableBlock(const std::size_t bytes);

		void Coalesce();

		free_list_node* Split(free_list_node* head);
		free_list_node* Merge(free_list_node* first, free_list_node* second);
		free_list_node* MergeSort(free_list_node* head);
		void print_list() const;

	public:
		free_list(const std::uint64_t capacity);
		~free_list();

		vAddress Allocate(const std::size_t bytes);
		void	 Deallocate(void* ptr);
	};
}

#endif // ! ALLOCATOR_LIB_FREE_LIST_H