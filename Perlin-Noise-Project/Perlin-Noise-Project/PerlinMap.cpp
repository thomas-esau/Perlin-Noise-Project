#include "PerlinMap.h"


template <typename T>
void PerlinMap<T>::calcMap()
{
	for (int i = 0; i < SUBSECTIONS_Y; i++)
	{

		for (int j = 0; j < SUBSECTIONS_X; j++)
		{
			if (mapParts[i].at(j) == NULL) continue;
			//std::cout << "createGridPart" << "\n";
			std::cout << "Adress: "  <<mapParts[i][j] << "\n";
			mapParts[i].at(j)->createGridPart(TOTAL_SIZE_X, TOTAL_SIZE_Y);
			//mapParts[i][j].createGrid();
			std::string filename = "x" + std::to_string(j) + "y" + std::to_string(i);
			mapParts[i].at(j)->dumpGrid();
			mapParts[i].at(j)->createFile(filename, "obj", params);
			mapParts[i].at(j)->createFile(filename, "ppm", params);
		}
	}

};

template <typename T>
void PerlinMap<T>::threadTask(uint32_t i)
{
	uint32_t x = i % SUBSECTIONS_X;
	uint32_t y = (int)(i / SUBSECTIONS_X);
	if (mapParts[y].at(x) == NULL) return;
	//std::cout << "Worker has been started. ID: " << std::this_thread::get_id() << "\n";
	mapParts[y].at(x)->createGridPart(TOTAL_SIZE_X, TOTAL_SIZE_Y);
	std::string filename = "x" + std::to_string(x) + "y" + std::to_string(y);
	//mapParts[y].at(x)->dumpGrid();
	//mapParts[y].at(x)->createFile(filename, "ppm" , params);
	mapParts[y].at(x)->createFile(filename, "obj" , params);
};

template <typename T>
void PerlinMap<T>::threadWorker()
{

	for (int i = 0; i < SUBSECTIONS_X * SUBSECTIONS_Y; i++)
	{
			worker[i] = std::thread((&PerlinMap<T>::threadTask),this, i);
	}

	for (int i = 0; i < SUBSECTIONS_X * SUBSECTIONS_Y; i++)
	{
			worker[i].join();
	}
};