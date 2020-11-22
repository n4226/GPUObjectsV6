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
	usedSize += size;
	assert(usedSize <= totalSize);

	auto out = currentAdress;
	currentAdress += size;

	return out;
}

void VaribleIndexAllocator::free(VkDeviceAddress address, VkDeviceSize size)
{

}
