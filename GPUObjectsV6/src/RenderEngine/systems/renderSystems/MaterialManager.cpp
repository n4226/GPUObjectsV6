#include "pch.h"
#include "MaterialManager.h"
#include "Application/FileManager.h"



MaterialManager::MaterialManager(Renderer& renderer)
	: renderer(renderer)
{

}

void MaterialManager::loadStatic()
{
	auto matRootPath = FileManager::getMaterialDir() + "staticAlloc/";

	// load each static material from disk into buffers then copy to images using resource transfer manager
	
	// load all mats
	loadMat(matRootPath, "grass1");


	renderer.resouceTransferer->newTask(pendingTasks, []() {

		}, false);
	pendingTasks.clear();

}

void MaterialManager::loadMat(std::string& matRootPath, const char* matFolder)
{
	auto path = matRootPath + matFolder + "/" + matFolder + "-albedo3.png";

	auto [buffer, image] = loadTex(path.c_str());

	images.push_back(image);
	buffers.push_back(buffer);

	addCopyToTasks(buffer, image);
}

void MaterialManager::addCopyToTasks(Buffer* buffer, Image* image)
{
	ResourceTransferer::BufferToImageCopyWithTransitionTask layoutTask{};

	layoutTask.buffer = buffer->vkItem;
	layoutTask.image = image->vkItem;
	layoutTask.oldLayout = vk::ImageLayout::eUndefined;
	layoutTask.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	layoutTask.imageAspectMask = vk::ImageAspectFlagBits::eColor;
	layoutTask.imageSize = image->size;

	ResourceTransferer::Task task = { ResourceTransferer::TaskType::bufferToImageCopyWithTransition };
	task.bufferToImageCopyWithTransitionTask = layoutTask;

	pendingTasks.push_back(task);
}

std::tuple<Buffer*, Image*> MaterialManager::loadTex(const char* path)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_default);

	VkDeviceSize imageBufferSize = texWidth * texHeight * texChannels;

	BufferCreationOptions bufferOptions =
	{ ResourceStorageType::cpuToGpu, { vk::BufferUsageFlagBits::eTransferSrc }, vk::SharingMode::eConcurrent,
	{ renderer.queueFamilyIndices.graphicsFamily.value(), renderer.queueFamilyIndices.resourceTransferFamily.value() } };

	auto buff = new Buffer(renderer.device, renderer.allocator, imageBufferSize, bufferOptions);

	buff->tempMapAndWrite(pixels);

	stbi_image_free(pixels);


	ImageCreationOptions imageOptions{};

	imageOptions.sharingMode = vk::SharingMode::eConcurrent;
	imageOptions.sharingQueueFamilieIndicies = { renderer.queueFamilyIndices.graphicsFamily.value(), renderer.queueFamilyIndices.resourceTransferFamily.value() };
	imageOptions.storage = ResourceStorageType::gpu;
	imageOptions.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;

	imageOptions.type = vk::ImageType::e2D;
	imageOptions.layout = vk::ImageLayout::eUndefined;
	imageOptions.tilling = vk::ImageTiling::eOptimal;


	imageOptions.format = vk::Format::eR8G8B8A8Srgb;

	vk::Extent3D imageSize = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),1 };

	auto image = new Image(renderer.device, renderer.allocator, imageSize, imageOptions, vk::ImageAspectFlagBits::eColor);


	return { buff, image };



}
