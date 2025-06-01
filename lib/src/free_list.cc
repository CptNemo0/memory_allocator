#include "free_list.h"

#include <cstring>
#include <format>
#include <iostream>
#include <new>
#include <stdexcept>
#include <stdlib.h>

#include "allocator_utility.h"

namespace allocator
{
	free_list::free_list(const std::uint64_t capacity)
	{
		capacity_ = capacity;
		data_ = malloc(capacity);

		if (!data_)
		{
			throw std::bad_alloc();
		}
		
		std::memset(data_, 0, capacity);

		head_ = static_cast<free_list_node*>(data_);
		head_->size = capacity - kFreeListNodeSize;
		head_->next = nullptr;

		std::cout << std::format("Head allocated at: {}\n", Rcast<uAddress>(head_));
	}

	free_list::~free_list()
	{
		free(data_);
	}

	std::pair<free_list_node*, free_list_node*> free_list::FindSuitableBlock(const std::size_t bytes)
	{
		const auto minimum_bytes_num = bytes + sizeof(header) + sizeof(free_list_node);

		// If the free list has an only one node and the size of that node is less than the minimum required bytes, return two nullptr
		if (!head_->next && head_->size < minimum_bytes_num) return { nullptr, nullptr };

		free_list_node* previous = nullptr;
		free_list_node* current = head_;

		while (current)
		{
			// If the current node has more free space than the minimum required bytes break
			if (current->size >= minimum_bytes_num) break;

			// Else move to the next node in the free list
			previous = current;
			current = current->next;
		}

		return { previous, current };
	}


	free_list_node* free_list::Split(free_list_node* head)
	{
		free_list_node* slow = head;
		free_list_node* fast = head;

		while (fast && fast->next)
		{
			fast = fast->next->next;
			if (fast) slow = slow->next;
		}

		free_list_node* second = slow->next;
		slow->next = nullptr;

		return second;
	}

	free_list_node* free_list::Merge(free_list_node* first, free_list_node* second)
	{
		if (!first) return second;
		if (!second) return first;

		auto first_address = Rcast<uAddress>(first);
		auto second_address = Rcast<uAddress>(second);

		if (first_address < second_address)
		{
			first->next = Merge(first->next, second);
			return first;
		}
		else
		{
			second->next = Merge(first, second->next);
			return second;
		}

		return nullptr;
	}

	free_list_node* free_list::MergeSort(free_list_node* head)
	{
		if (!head || !head->next) return head;

		free_list_node* second = Split(head);

		head = MergeSort(head);
		second = MergeSort(second);

		return Merge(head, second);
	}

	void free_list::print_list() const
	{
		std::cout << "Free List Start\n";
		free_list_node* current = head_;

		while (current)
		{
			std::cout << std::format("\tAddress: {}, Size: {}\n", Rcast<uAddress>(current), current->size);
			current = current->next;
		}

		std::cout << "Free List End\n";
	}


	// After freeing something we should coalsce the free list to avoid fragmentation
	// Coalescing means merging adjacent free blocks into a single larger block
	void free_list::Coalesce()
	{
		free_list_node* current = head_;

		while (current && current->next)
		{
			auto current_u_address = ToUAddress(current);
			auto correct_next_address = current_u_address + current->size + kFreeListNodeSize;
			auto next_u_address = ToUAddress(current->next);

			// If the next node is adjacent to the current node, merge them
			if (next_u_address == correct_next_address)
			{
				std::cout << std::format("Merging {} and {}\n", Rcast<uAddress>(current), Rcast<uAddress>(current->next));
				current->size += current->next->size + kFreeListNodeSize;		// Update the size of the current node
				current->next->size = 0;										// Zero out the memory
				current->next = std::exchange(current->next->next, nullptr);    // Zero out the memory
			}
			else
			{
				current = current->next; // Move to the next node
			}
		}
	}

	void* free_list::Allocate(const std::size_t bytes)
	{
		if (!head_) throw std::logic_error("For some reason, head of the free list was not allocated.");
		if (bytes > capacity_) throw std::logic_error(std::format("Trying to allocate {} bytes, when the maximum capacity is {} bytes", bytes, capacity_));
		if (!bytes) return nullptr;

		const auto [previous, current] = FindSuitableBlock(bytes);

		if (!current) throw std::bad_alloc();

		// At this point it is certain that the current node has enough space to 
		// allocate requested bytes, header bytes, and free list node bytes.
		// This is not the most memory effiecient, but it's a simplified policy.
		// Should use strategy design pattern to ensure extensibility in future.

		const auto u_address = ToUAddress(current);
		const auto return_value = ToVAddress(u_address + kHeaderSize);
		
		const free_list_node current_copy { current->next, current->size };

		const bool current_isnt_null = current;
		const bool current_is_head = (current == head_);
		const bool current_next_isnt_null = current->next;

		free_list_node* new_node = std::launder(AllocateHeader(u_address, bytes)); // This line invalidates current pointer, that's why we copy it's data
		new_node->size = current_copy.size - (bytes + kHeaderSize);
		if (previous && current_isnt_null)
		{
			previous->next = new_node;
			if (current_next_isnt_null)
			{
				new_node->next = current_copy.next;
			}
		}
		else if (!previous && current_isnt_null)
		{
			if (current_next_isnt_null)
			{
				new_node->next = current_copy.next;
			}
		}
		
		if (current_is_head)
		{
			std::cout << std::format("Moving head from {} to {}\n", Rcast<uAddress>(head_), Rcast<uAddress>(new_node));
			head_ = new_node;
		}

		std::memset(return_value, 0, bytes);
		print_list();
		return return_value;
	}

	void free_list::Deallocate(vAddress ptr)
	{
		const uAddress u_address = ToUAddress(ptr);
		const uAddress header_u_address = u_address - kHeaderSize;
		const vAddress header_v_address = ToVAddress(header_u_address);

		// Retrieve the header pointer from the address
		header* header_ptr = std::launder(static_cast<header*>(header_v_address));
		const auto full_size = header_ptr->size + kHeaderSize;

		// Check if the magic number matches
		if (header_ptr->magic != kMagicNumber)
		{
			throw std::logic_error("Trying to deallocate memory that was not allocated by this allocator. Alternatively memmory corruption error");
		}

		// Zero the memory of the header and allocated memory
		std::memset(header_v_address, 0, header_ptr->size + kHeaderSize);
		// Don't access header_ptr after this point

		// Create a new free list node at the header address
		free_list_node* new_node = std::launder(static_cast<free_list_node*>(header_v_address));
		new_node->size = full_size - kFreeListNodeSize;
		new_node->next = head_; // New node will become a head so track the previous head
		std::cout << std::format("Moving head from {} to {}\n", Rcast<uAddress>(head_), Rcast<uAddress>(new_node));
		head_ = new_node;
		head_ = MergeSort(head_); // Sort the free list to ensure adjacent blocks are next to each other

		Coalesce();
		print_list();
		return;
	}
}