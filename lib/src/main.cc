#include <array>
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include "allocator_utility.h"
#include "free_list.h"

int main()
{	
	const std::uint64_t allocated_bytes = 4096000u;

	allocator::free_list free_list(allocated_bytes);
	auto a = free_list.Allocate(1000);
	std::cout << "Allocated A memory at: " << allocator::ToUAddress(a) << "\n";
	
	auto b = free_list.Allocate(1000);
	std::cout << "Allocated B memory at: " << allocator::ToUAddress(b) << "\n";
	
	auto c = free_list.Allocate(1000);
	std::cout << "Allocated C memory at: " << allocator::ToUAddress(c) << "\n";

	std::cout << "Deallocating A \n";
	free_list.Deallocate(a);

	std::cout << "Deallocating B \n";
	free_list.Deallocate(b);

	auto d = free_list.Allocate(1000);
	std::cout << "Allocated D memory at: " << allocator::ToUAddress(d) << "\n";

	std::cout << "Deallocating D \n";
	free_list.Deallocate(d);
	
	return 0;
}
