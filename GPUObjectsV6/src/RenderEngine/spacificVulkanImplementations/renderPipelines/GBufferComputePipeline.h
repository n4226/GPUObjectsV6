#pragma once

#include "../../../pch.h"
#include "ComputePipeline.h"

class DeferredPass: public GraphicsPipeline
{
public:
	//GBufferPass(vk::Device device);
	//~GBufferPass();

	void createPipeline() override;

	using GraphicsPipeline::GraphicsPipeline;

};

