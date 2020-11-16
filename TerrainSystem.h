#pragma once

#include "RenderSystem.h"

class TerrainSystem: public RenderSystem
{

	void update() override;
	void renderSystem(vk::CommandBuffer buffer) override;

};

