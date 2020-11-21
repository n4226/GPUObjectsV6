#include "pch.h"
#include "IndexAllocator.h"

IndexAllocator::IndexAllocator(VkDeviceSize size, VkDeviceSize allocSize)
	: totalSize(size), allocSize(allocSize)
{

}

IndexAllocator::~IndexAllocator()
{

}

VkDeviceAddress IndexAllocator::alloc()
{
	return (currentIndex++) * allocSize;
}

void IndexAllocator::free(VkDeviceAddress index)
{

}
