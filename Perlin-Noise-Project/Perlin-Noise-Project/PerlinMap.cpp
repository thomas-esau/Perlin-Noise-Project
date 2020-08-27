#include "PerlinMap.h"

void PerlinMap::calcMapPart(uint32_t i)
{
	int j = mapParts[i]->getObjectID() - 1;
	uint32_t x = j % SUBSECTIONS_X;
	uint32_t y = (int)(j / SUBSECTIONS_X);

	mapParts[i]->createGridPart(TOTAL_SIZE_X, TOTAL_SIZE_Y);
	std::string filename = "x" + std::to_string(x) + "y" + std::to_string(y);
	//mapParts[i]->createFile(filename, "ppm" , params);
	mapParts[i]->createFile(filename, "obj", params);
};

void PerlinMap::threadWorker()
{

	for (int i = 0; i < mapParts.size(); i++) {
			worker[i] = std::thread(&PerlinMap::calcMapPart,this, i);
	}

	for (int i = 0; i < mapParts.size(); i++) {
			worker[i].join();
	}
};