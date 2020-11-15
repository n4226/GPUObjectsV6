#pragma once

#include <vulkan/vulkan.hpp>
#include "GPUSelector.h"
#include "ApplicationWindow.h"
#include "Buffer.h"
#include "Mesh.h"
#include "uniforms.h"
#include "Transform.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Camera.h"
#include "PipelineCreator.h"
#include <chrono>

class Renderer
{
public:

	Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window);
	~Renderer();

	void renderFrame();

private:

	void createRenderResources();

	void createDescriptorPoolAndSets();

	void createStaticRenderCommands();

	vk::Device& device;
	vk::PhysicalDevice& physicalDevice;
	WindowManager& window;

	// render resources

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	VmaAllocator allocator;

	vk::DescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	// temp vars

	Buffer* vertBuffer;
	Buffer* indexBuffer;

	Mesh* mesh;
	MeshBuffer* meshBuffer;

	std::vector<Buffer*> uniformBuffers;
};

