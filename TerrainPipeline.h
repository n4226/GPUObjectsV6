#pragma once

#include "PipelineCreator.h"

class TerrainPipeline: public PipelineCreator
{
public:
	using PipelineCreator::PipelineCreator;
	void createGraphicsPipeline() override;
protected:
};

