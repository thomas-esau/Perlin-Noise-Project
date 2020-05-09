#include "PerlinMap.h"


template <typename T>
void PerlinMap<T>::calcMap()
{
	for (int i = 0; i < SUBSECTIONS_Y; i++)
	{

		for (int j = 0; j < SUBSECTIONS_X; j++)
		{
			mapPart[j][i].createGridPart(SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y);
			//mapPart[j][i].createGrid();
			std::string filename = "x" + std::to_string(j) + "y" + std::to_string(i);
			mapPart[j][i].dumpGrid();
			mapPart[j][i].createPPMFile(filename, mod);
		}
	}

};
//template class PerlinMap<PerlinNoise2D_MEM>
