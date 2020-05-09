#pragma once
#include <cstdint>
#include "PerlinNoise.h"
#include <vector>

template <typename T>
class PerlinMap
{
public:

	PerlinMap(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SUBSECTIONS_X, uint32_t init_SUBSECTIONS_Y, uint32_t init_SEED) :
		SIZE_X{ init_SIZE_X }, SIZE_Y{ init_SIZE_Y }, SUBSECTIONS_X{ init_SUBSECTIONS_X }, SUBSECTIONS_Y{ init_SUBSECTIONS_Y }, SEED{ init_SEED }
	{
		//PerlinNoiseArray
		mod.scale_x = 1/16.;
		mod.scale_y = 1/16.;
		mod.amplitude = 0.5 * 8;
		mod.grayscales = 2 * mod.amplitude;
		mod.height = 1;

		part_size_x = SIZE_X / SUBSECTIONS_X * mod.scale_x;
		part_size_y = SIZE_Y / SUBSECTIONS_Y * mod.scale_y;

		constructMapParts();


	};

	void calcMap();
	bool isSplittedMap() { return SUBSECTIONS_X == 1 && SUBSECTIONS_Y == 1 ? false : true; };
	void setMods(PerlinModifiers setMod) { mod = setMod; };
	void constructMapParts()
	{
		for (int i = 0; i < SUBSECTIONS_Y; i++)
		{
			std::vector<T> row;
			for (int j = 0; j < SUBSECTIONS_X; j++)
			{
				row.push_back(T(part_size_x, part_size_y, part_size_x / SUBSECTIONS_X * j, part_size_y / SUBSECTIONS_Y * i, SEED));
				std::string filename = "x" + std::to_string(j) + "y" + std::to_string(i);
				std::cout << row[j].toString() << "\n";
			}
			mapPart.push_back(row);
		}

	};
	void deconstructMapParts()
	{
		mapPart.clear();

	};

private:
	uint32_t part_size_x;
	uint32_t part_size_y;
	PerlinModifiers mod;
	//T** mapPart;
	std::vector<std::vector<T>> mapPart;
	const uint32_t SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED;
};

