#pragma once

#include "PipelineCreator.h"

class TrianglePipeline : public PipelineCreator
{
public:
	using PipelineCreator::PipelineCreator;
	void createGraphicsPipeline() override;
protected:
};

