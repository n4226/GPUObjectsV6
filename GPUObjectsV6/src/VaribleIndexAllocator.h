#pragma once
#include "pch.h"

class VaribleIndexAllocator
{
public:
	VaribleIndexAllocator(VkDeviceSize size);

	~VaribleIndexAllocator();

	/// <summary>
	/// returns UINT64_MAX if full
	/// </summary>
	/// <returns></returns>
	VkDeviceAddress alloc(VkDeviceSize size);
	void free(VkDeviceAddress address, VkDeviceSize size);


	const VkDeviceSize totalSize;
	VkDeviceSize allocatedSize = 0;

	struct freeSpace {
		VkDeviceSize start;
		VkDeviceSize size;

		bool operator<(const freeSpace& other) const {
			return size < other.size;
		}
	};

	struct freeSpace_rank_greater_than {
		bool operator()(freeSpace const* a, freeSpace const* b) const {
			return a->size < b->size;
		}
	};

	void addFreeSpace(VaribleIndexAllocator::freeSpace* newSpace);

private:

	std::vector<freeSpace*> freeSpaces = {};
	std::unordered_map<VkDeviceSize,freeSpace*> freeSpaces_ends = {};
	std::unordered_map<VkDeviceSize,freeSpace*> freeSpaces_beginnings = {};

	std::unordered_set<freeSpace*> noLongerFreeSpaceHitList = {};

	VkDeviceAddress currentAdress = 0;
};

