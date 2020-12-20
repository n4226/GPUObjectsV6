#include "pch.h"
#include "Image.h"




Image::Image(vk::Device device, VmaAllocator allocator, vk::Extent3D size, ImageCreationOptions options,vk::ImageAspectFlags aspectFlags)
	: size(size), allocator(allocator), device(device)
{

	vk::ImageCreateInfo imageInfo{};
	imageInfo.imageType = options.type;
	imageInfo.extent = size;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;

	imageInfo.format = options.format;

	imageInfo.tiling = options.tilling;

	imageInfo.initialLayout = options.layout;

	imageInfo.usage = options.usage;
	imageInfo.sharingMode = options.sharingMode;

	// currently abstracted out of image creation

	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.flags = {};

	if (options.sharingMode == vk::SharingMode::eConcurrent)
		imageInfo.setQueueFamilyIndices(options.sharingQueueFamilieIndicies);

	VkImageCreateInfo c_imageInfo = imageInfo;

	// allocate

	VmaAllocationCreateInfo allocInfo;

	allocInfo.flags = 0;//VMA_ALLOCATION_CREATE_MAPPED_BIT;
	allocInfo.preferredFlags = 0;
	allocInfo.requiredFlags = 0;
	allocInfo.memoryTypeBits = UINT32_MAX;
	allocInfo.pool = nullptr;
	allocInfo.pUserData = nullptr;
	allocInfo.usage = VmaMemoryUsage(options.storage);


	vmaCreateImage(allocator, &c_imageInfo, &allocInfo, &vkItem, &allocation, nullptr);

	ImageViewCreationOptions viewOptions = { vk::ImageViewType::e2D, options.format, aspectFlags };

	view = VkHelpers::createImageView(device, vkItem, viewOptions);
}

Image::~Image()
{
	device.destroyImageView(view);
	vmaDestroyImage(allocator, vkItem, allocation);
}
