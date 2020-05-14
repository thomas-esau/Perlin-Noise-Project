#include "PerlinMap.h"


template <typename T>
void PerlinMap<T>::calcMap()
{
	for (int i = 0; i < SUBSECTIONS_Y; i++)
	{

		for (int j = 0; j < SUBSECTIONS_X; j++)
		{
			//std::cout << "createGridPart" << "\n";
			mapPart[i][j].createGridPart(SIZE_X, SIZE_Y);
			//mapPart[i][j].createGrid();
			std::string filename = "x" + std::to_string(j) + "y" + std::to_string(i);
			mapPart[i][j].dumpGrid();
			mapPart[i][j].createPPMFile(filename, mod);
		}
	}

};
//template class PerlinMap<PerlinNoise2D_MEM>

template <typename T>
void PerlinMap<T>::threadTask(uint32_t i)
{
	uint32_t x = i % SUBSECTIONS_X;
	uint32_t y = (int)(i / SUBSECTIONS_X);

	std::cout << "Worker has been started. ID: " << std::this_thread::get_id() << "\n";
	mapPart[y][x].createGridPart(SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y);
	std::string filename = "x" + std::to_string(x) + "y" + std::to_string(y);
	mapPart[y][x].dumpGrid();
	mapPart[y][x].createPPMFile(filename, mod);
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