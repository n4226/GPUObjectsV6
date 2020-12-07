#include "pch.h"
#include "VaribleIndexAllocator.h"

#include <algorithm>

VaribleIndexAllocator::VaribleIndexAllocator(VkDeviceSize size)
	: totalSize(size)
{
	auto fullSpace = new freeSpace;

	fullSpace->start = 0;
	fullSpace->size = size;

	freeSpaces.push_back(fullSpace);
	freeSpaces_beginnings.insert(std::pair<VkDeviceSize,freeSpace*>(0,fullSpace));
	freeSpaces_ends.insert(std::pair<VkDeviceSize,freeSpace*>(size - 1,fullSpace));
	std::make_heap(freeSpaces.begin(), freeSpaces.end(), freeSpace_rank_greater_than());
}

VaribleIndexAllocator::~VaribleIndexAllocator()
{

}

VkDeviceAddress VaribleIndexAllocator::alloc(VkDeviceSize size)
{

	freeSpace* space = nullptr;

	bool loop = true;

	while (loop)
	{
		std::pop_heap(freeSpaces.begin(), freeSpaces.end(), freeSpace_rank_greater_than());
		space = freeSpaces[freeSpaces.size() - 1];
		freeSpaces.pop_back();
		freeSpaces_ends.erase(space->start + space->size - 1);
		freeSpaces_beginnings.erase(space->start);

		if (noLongerFreeSpaceHitList.count(space) == 0)
			loop = false;
		else {
			//noLongerFreeSpaceHitList.erase(space);
			delete space;
		}
	}

	assert(space->size >= size);

	allocatedSize += size;

	auto newSpace = new freeSpace;

	newSpace->start = space->start + size;
	newSpace->size = space->size - size;

	addFreeSpace(newSpace);


	std::cout << "allocated: " << allocatedSize << "/" << totalSize << " = " << static_cast<float>(allocatedSize) / (totalSize / 100) << "%" << std::endl;


	auto start = space->start;

	delete space;

	return start;


	/* pre free list heap system 

	allocatedSize += size;
	//std::cout << "allocated: " << allocatedSize << "/" << totalSize << " = " << static_cast<float>(allocatedSize) / (totalSize / 100) << "%" << std::endl;
	assert(allocatedSize <= totalSize);

	auto out = currentAdress;
	currentAdress += size;

	return out;
	*/
}

void VaribleIndexAllocator::free(VkDeviceAddress address, VkDeviceSize size)
{

	

	auto newSpace = new freeSpace;

	newSpace->start = address;
	newSpace->size = size;


	if (freeSpaces_ends.count(address - 1) > 0) {
		auto previous = freeSpaces_ends.at(address - 1);

		noLongerFreeSpaceHitList.insert(previous);

		newSpace->start = previous->start;
		newSpace->size += previous->size;
	}

	if (freeSpaces_beginnings.count(address + size) > 0) {
		auto next = freeSpaces_beginnings.at(address + size);

		noLongerFreeSpaceHitList.insert(next);

		newSpace->size += next->size;
	}

	addFreeSpace(newSpace);


	allocatedSize -= size;

}

void VaribleIndexAllocator::addFreeSpace(VaribleIndexAllocator::freeSpace* newSpace)
{
	freeSpaces.push_back(newSpace);
	std::push_heap(freeSpaces.begin(), freeSpaces.end(),freeSpace_rank_greater_than());
	freeSpaces_beginnings.insert(std::pair<VkDeviceSize, freeSpace*>(newSpace->start, newSpace));
	freeSpaces_ends.insert(std::pair<VkDeviceSize, freeSpace*>(newSpace->start + newSpace->size - 1, newSpace));
}
