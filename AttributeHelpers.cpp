#include "AttributeHelpers.h"

VkVertexInputBindingDescription makeVertBinding(uint32_t index, uint32_t stride, vk::VertexInputRate inputRate)
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = index;
	bindingDescription.stride = stride;
	bindingDescription.inputRate = VkVertexInputRate(inputRate);

	return bindingDescription;
}

VkVertexInputAttributeDescription makeVertAttribute(uint32_t binding, uint32_t location, VertexAttributeFormat format, uint32_t offset)
{
	return vk::VertexInputAttributeDescription(location, binding, vk::Format(format), offset);
}
