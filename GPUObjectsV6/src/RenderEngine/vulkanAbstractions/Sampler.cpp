#include "pch.h"
#include "Sampler.h"

Sampler::Sampler(vk::Device device, CreateOptions& options)
	: device(device)
{

	vk::SamplerCreateInfo samplerInfo{};
	samplerInfo.magFilter = options.magFilter;
	samplerInfo.minFilter = options.minFilter;

	samplerInfo.addressModeU = options.addressModeU;
	samplerInfo.addressModeV = options.addressModeV;
	samplerInfo.addressModeW = options.addressModeW;

	samplerInfo.anisotropyEnable = options.enableAnisotropy;
	samplerInfo.maxAnisotropy = options.maxAnisotropy;

	samplerInfo.borderColor = options.borderColor;

	samplerInfo.unnormalizedCoordinates = options.unnormalizedCoordinates;

	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	vkItem = device.createSampler(samplerInfo);

}

Sampler::~Sampler()
{
	device.destroySampler(vkItem);
}

void Sampler::CreateOptions::setFilterAll(vk::Filter filter)
{
	magFilter = filter;
	minFilter = filter;
}

void Sampler::CreateOptions::setAdressModeAll(vk::SamplerAddressMode addressMode)
{
	addressModeU = addressMode;
	addressModeV = addressMode;
	addressModeW = addressMode;
}
