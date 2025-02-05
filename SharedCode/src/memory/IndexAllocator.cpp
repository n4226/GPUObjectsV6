#include "pch.h"

#include "IndexAllocator.h"
#include <assert.h>


IndexAllocator::IndexAllocator(size_t indexCount, size_t allocSize)
	: totalSize(indexCount * allocSize), indexCount(indexCount), allocSize(allocSize)
{

}

IndexAllocator::~IndexAllocator()
{

}

size_t IndexAllocator::alloc()
{
	assert(currentIndex * allocSize < totalSize);
	return (currentIndex++) * allocSize;
}

void IndexAllocator::free(size_t index)
{

}
