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
		vAddress data_ = nullptr;		// Pointer to the allocated memory block, used for RAII

		std::uint64_t capacity_;		// Total capacity of the free list
		free_list_node* head_;			// Pointer to the head of the free list
		
		std::pair<free_list_node*, free_list_node*> FindSuitableBlock(const std::size_t bytes);

		free_list_node* Split(free_list_node* head);
		free_list_node* Merge(free_list_node* first, free_list_node* second);
		free_list_node* MergeSort(free_list_node* head);
		
		void Coalesce();

		void PrintList() const;

	public:
		free_list(const std::uint64_t capacity);
		~free_list();

		free_list(const free_list&) = delete;
		free_list& operator=(const free_list&) = delete;

		free_list(free_list&& other) noexcept : capacity_(other.capacity_), head_(other.head_), data_(other.data_) {};
		free_list& operator=(free_list&&) noexcept;

		vAddress Allocate(const std::size_t bytes);
		void Deallocate(vAddress ptr);
	};
}

#endif // ! ALLOCATOR_LIB_FREE_LIST_H