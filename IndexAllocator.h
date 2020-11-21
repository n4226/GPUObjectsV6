#pragma once
#include "pch.h"
#include <set>

/// <summary>
/// keeps track of indices of entries with an index and fixed size in collection of memory
/// </summary>
class IndexAllocator
{
public:
	IndexAllocator(VkDeviceSize size,VkDeviceSize allocSize);

	~IndexAllocator();
	
	/// <summary>
	/// returns UINT64_MAX if full
	/// </summary>
	/// <returns></returns>
	VkDeviceAddress alloc();
	void free(VkDeviceAddress index);

	const VkDeviceSize totalSize;
	const VkDeviceSize allocSize;
	VkDeviceSize usedSize = 0;


private:

	/// <summary>
	/// in unscaled space - all indicies a next to eachother = output space / allocSize
	/// </summary>
	//std::set<VkDeviceAddress> usedIndicies;

	VkDeviceAddress currentIndex;

};

