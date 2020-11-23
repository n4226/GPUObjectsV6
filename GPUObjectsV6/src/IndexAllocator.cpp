#include "pch.h"
#include "IndexAllocator.h"

IndexAllocator::IndexAllocator(VkDeviceSize indexCount, VkDeviceSize allocSize)
	: totalSize(indexCount * allocSize), indexCount(indexCount), allocSize(allocSize)
{

}

IndexAllocator::~IndexAllocator()
{

}

VkDeviceAddress IndexAllocator::alloc()
{
	assert(currentIndex * allocSize < totalSize);
	return (currentIndex++) * allocSize;
}

void IndexAllocator::free(VkDeviceAddress index)
{

}
