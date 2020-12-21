#pragma once

#include "pch.h"



class Sampler
{
public:

	struct CreateOptions
	{
		vk::Filter magFilter = vk::Filter::eLinear;
		vk::Filter minFilter = vk::Filter::eLinear;

		void setFilterAll(vk::Filter filter);

		vk::SamplerAddressMode addressModeU = vk::SamplerAddressMode::eRepeat;
		vk::SamplerAddressMode addressModeV = vk::SamplerAddressMode::eRepeat;
		vk::SamplerAddressMode addressModeW = vk::SamplerAddressMode::eRepeat;

		void setAdressModeAll(vk::SamplerAddressMode addressMode);

		bool enableAnisotropy = true;
		//TODO: ix defualt value
		float maxAnisotropy = 8;

		vk::BorderColor borderColor = vk::BorderColor::eFloatOpaqueBlack;
		
		bool unnormalizedCoordinates = false;

		// mip mapping comming soon - month: TBD, day: TBD, year: 2021 

		//mipmapMode

	};


	Sampler(vk::Device device, CreateOptions& options);
	~Sampler();

	vk::Sampler vkItem;

private:

	vk::Device device;

};

