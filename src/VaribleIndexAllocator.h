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
	void free(VkDeviceAddress address,VkDeviceSize size);

	const VkDeviceSize totalSize;
	VkDeviceSize usedSize = 0;

private:

	VkDeviceAddress currentAdress;
};

