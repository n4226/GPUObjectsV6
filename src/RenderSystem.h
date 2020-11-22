#pragma once
#include "environment.h"
#include "pch.h"

class System {
	virtual void update() = 0;
};

class RenderSystem: public System
{
	/// <summary>
	/// called on the update thread
    /// requests the system to render its contents into a given render command buffer using a dispatchQueue and to return when it has finished
    /// system might be able to do this in parelel
	/// </summary>
	/// <param name="buffer"></param>
	virtual vk::CommandBuffer* renderSystem(uint32_t subpass) = 0;
};

