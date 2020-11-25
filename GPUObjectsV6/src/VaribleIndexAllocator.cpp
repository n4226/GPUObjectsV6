#include "pch.h"
#include "VaribleIndexAllocator.h"

VaribleIndexAllocator::VaribleIndexAllocator(VkDeviceSize size)
	: totalSize(size)
{

}

VaribleIndexAllocator::~VaribleIndexAllocator()
{
}

VkDeviceAddress VaribleIndexAllocator::alloc(VkDeviceSize size)
{
	allocatedSize += size;
	std::cout << "allocated: " << allocatedSize << "/" << totalSize << " = " << static_cast<float>(allocatedSize) / (totalSize / 100) << "%" << std::endl;
	assert(allocatedSize <= totalSize);

	auto out = currentAdress;
	currentAdress += size;

	return out;
}

void VaribleIndexAllocator::free(VkDeviceAddress address, VkDeviceSize size)
{

}
