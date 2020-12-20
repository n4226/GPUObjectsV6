#pragma once

#include "pch.h"
#include "Buffer.h"


struct ImageCreationOptions {
	// resource spacific
	ResourceStorageType storage;
	vk::ImageUsageFlags usage;
	vk::SharingMode sharingMode;// = vk::SharingMode::eExclusive;

	// image spacific
	vk::ImageLayout layout;// = vk::ImageLayout::eUndefined;
	vk::ImageType type;
	vk::ImageTiling tilling;// = vk::ImageTiling::eOptimal;
	vk::Format format;
};

struct ImageViewCreationOptions {
	vk::ImageViewType type;
	vk::Format format;
	vk::ImageAspectFlags aspectFlags;
};

class Image
{
public:
	
	Image(const Image& othter) = delete;
	Image(vk::Device device, VmaAllocator allocator, vk::Extent3D size, ImageCreationOptions options, vk::ImageAspectFlags aspectFlags);
	~Image();

	vk::Extent3D size;
	VmaAllocation allocation = nullptr;
	VkImage vkItem = nullptr;

	vk::ImageView view;

private:
	VmaAllocator allocator;
	vk::Device device;
};
