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

	std::vector<Image*>  images;
	std::vector<Buffer*> buffers;
	std::vector<ResourceTransferer::Task> pendingTasks;

 	std::tuple<Buffer*,Image*> loadTex(const char* path);

	void addCopyToTasks(Buffer* buffer, Image* image);


	Renderer& renderer;
};
