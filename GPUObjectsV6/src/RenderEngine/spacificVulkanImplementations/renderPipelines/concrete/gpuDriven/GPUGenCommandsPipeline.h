#pragma once

#include "pch.h"

class Application;

class GPUGenCommandsPipeline
{
public:
	GPUGenCommandsPipeline(Application& app, vk::Device device, GraphicsPipeline& terrainPipeline);


	VkIndirectCommandsLayoutNV commandsLayout;

	Buffer* commandsBuffer;

};

