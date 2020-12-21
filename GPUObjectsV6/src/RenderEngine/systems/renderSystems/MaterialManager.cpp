#include "pch.h"
#include "MaterialManager.h"
#include "Application/FileManager.h"
#include "Application/ApplicationRendererBridge/Application.h"



MaterialManager::MaterialManager(Renderer& renderer)
	: renderer(renderer)
{
	Sampler::CreateOptions options{};

	options.enableAnisotropy = true;

	matSampler = new Sampler(renderer.device,options);

}

void MaterialManager::loadStatic()
{
	PROFILE_FUNCTION;

	auto matRootPath = FileManager::getMaterialDir() + "staticAlloc/";

	// load each static material from disk into buffers then copy to images using resource transfer manager
	
	// load all mats
	loadMat(matRootPath, "grass1");




	renderer.resouceTransferer->newTask(pendingTasks, []() {

	}, true);
	pendingTasks.clear();

}

void MaterialManager::loadMat(std::string& matRootPath, const char* matFolder)
{
	PROFILE_FUNCTION;


	auto al_index = FinishLoadingTexture(loadTex((matRootPath + matFolder + "/" + matFolder + "-albedo3.png").c_str()));
	auto n_index  = FinishLoadingTexture(loadTex((matRootPath + matFolder + "/" + matFolder + "-normal1-ogl.png").c_str()));
	auto m_index  = FinishLoadingTexture(loadTex((matRootPath + matFolder + "/" + matFolder + "-metal.psd").c_str()));
	auto ao_index = FinishLoadingTexture(loadTex((matRootPath + matFolder + "/" + matFolder + "-ao.png").c_str()));
	//auto [r_buffer, r_image] = loadTex((matRootPath + matFolder + "/" + matFolder + "-albedo3.png").c_str());



	MaterialUniforms matInfor{};

	matInfor.baseTextureIndex = al_index;

	auto index = renderer.matUniformAllocator->alloc();

	renderer.globalMaterialUniformBufferStaging->tempMapAndWrite(&matInfor,0,sizeof(matInfor));

	//TODO: Transfer

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
	PROFILE_FUNCTION;


	BufferCreationOptions bufferOptions =
	{ ResourceStorageType::cpuToGpu, { vk::BufferUsageFlagBits::eTransferSrc }, vk::SharingMode::eConcurrent,
	{ renderer.queueFamilyIndices.graphicsFamily.value(), renderer.queueFamilyIndices.resourceTransferFamily.value() } };

	int texWidth, texHeight, texChannels;

	stbi_uc* pixels;
	//mango::u32* pixels;
	//{
		//PROFILE_SCOPE("stbi_load");
		pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_default);

		// $(SolutionDir)Dependencies\mango - master\mango - master\build\vs2019\x64\Debug\mango.lib
	
		/*mango::Bitmap bitmap(path, mango::Format(32, mango::Format::UNORM, mango::Format::BGRA, 8, 8, 8, 8));
		texWidth = bitmap.width;
		texHeight = bitmap.height;
		texChannels = 4;
		pixels = bitmap.address<mango::u32>(0, 0);*/

	//}

	assert(pixels != nullptr);

	VkDeviceSize imageBufferSize = texWidth * texHeight * texChannels;

	

	auto buff = new Buffer(renderer.device, renderer.allocator, imageBufferSize, bufferOptions);

	buff->tempMapAndWrite(pixels);

	//stbi_image_free(pixels);


	ImageCreationOptions imageOptions{};

	imageOptions.sharingMode = vk::SharingMode::eConcurrent;
	imageOptions.sharingQueueFamilieIndicies = { renderer.queueFamilyIndices.graphicsFamily.value(), renderer.queueFamilyIndices.resourceTransferFamily.value() };
	imageOptions.storage = ResourceStorageType::gpu;
	imageOptions.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;

	imageOptions.type = vk::ImageType::e2D;
	imageOptions.layout = vk::ImageLayout::eUndefined;
	imageOptions.tilling = vk::ImageTiling::eOptimal;


	imageOptions.format = vk::Format::eR8G8Unorm;

	vk::Extent3D imageSize = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),1 };

	auto image = new Image(renderer.device, renderer.allocator, imageSize, imageOptions, vk::ImageAspectFlagBits::eColor);


	return { buff, image };



}

glm::uint32 MaterialManager::FinishLoadingTexture(std::tuple<Buffer*, Image*> texture)
{
	PROFILE_FUNCTION;

	auto [buffer, image] = texture;

	images.push_back(image);
	buffers.push_back(buffer);

	addCopyToTasks(buffer, image);

	auto index = images.size() - 1;

	addTexToGlobal(image,index);

	return index;

}


//TODO: IMPORTANT - make this call update descriptors only once for a bunch of aditions ---------------------- IMPORTANT
void MaterialManager::addTexToGlobal(Image* image, glm::uint32 imageIndex)
{
	PROFILE_FUNCTION;

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = image->view;
	imageInfo.sampler = matSampler->vkItem;


	for (size_t window = 0; window < renderer.windows.size(); window++) {
		for (size_t i = 0; i < renderer.app.maxSwapChainImages; i++) {
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = renderer.descriptorSets[window][i];
			descriptorWrite.dstBinding = 3;
			descriptorWrite.dstArrayElement = imageIndex;

			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pBufferInfo = nullptr; // Optional
			descriptorWrite.pImageInfo = &imageInfo; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			renderer.device.updateDescriptorSets({ descriptorWrite }, {});

		}
	}
}
