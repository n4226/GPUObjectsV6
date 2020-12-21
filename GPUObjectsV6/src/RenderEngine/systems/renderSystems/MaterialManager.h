#pragma once

#include "pch.h"
#include "Renderer.h"

struct MaterialImages
{
	std::vector<Image*> albedoImages;
	std::vector<Image*> normalImages;
	std::vector<Image*> metallicImages;
	std::vector<Image*> roughnessImages;
	std::vector<Image*> aoImages;
};

class MaterialManager
{
public:
	MaterialManager(Renderer& renderer);

	void loadStatic();

	void loadMat(std::string& matRootPath, const char* matFolder);


private:

	Sampler* matSampler;

	std::vector<Image*>  images;
	std::vector<Buffer*> buffers;

	std::vector<ResourceTransferer::Task> pendingTasks;

 	std::tuple<Buffer*,Image*> loadTex(const char* path);

	glm::uint32 FinishLoadingTexture(std::tuple<Buffer*, Image*> texture);

	void addTexToGlobal(Image* image, glm::uint32 imageIndex);

	void addCopyToTasks(Buffer* buffer, Image* image);


	Renderer& renderer;
};

