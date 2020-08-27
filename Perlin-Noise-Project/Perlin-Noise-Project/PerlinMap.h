#pragma once
#include <cstdint>
#include "PerlinNoise.h"
#include <vector>
#include <thread>

class PerlinMap
{
public:

	PerlinMap(uint32_t init_TOTAL_SIZE_X, uint32_t init_TOTAL_SIZE_Y, uint32_t init_SUBSECTIONS_X, uint32_t init_SUBSECTIONS_Y, uint32_t discards, uint32_t amount , uint32_t init_SEED) :
		TOTAL_SIZE_X{ init_TOTAL_SIZE_X }, TOTAL_SIZE_Y{ init_TOTAL_SIZE_Y }, SUBSECTIONS_X{ init_SUBSECTIONS_X }, SUBSECTIONS_Y{ init_SUBSECTIONS_Y }, SEED{ init_SEED }
	{
		//PerlinParameters
		params.freq_x = 1/32.;
		params.freq_y = 1/32.;
		params.amplitude = 0.5;
		params.height = 1;

		constructMapParts(discards, amount);
	};

	bool isSplittedMap() { return SUBSECTIONS_X == 1 && SUBSECTIONS_Y == 1 ? false : true; };
	void setParams(PerlinParameters setParams) { params = setParams; };
	void constructMapParts(uint32_t discards, uint32_t amount)
	{


		for (int i = 0; i < SUBSECTIONS_X * SUBSECTIONS_Y; i++)
		{
			if (discards > 0)
			{
				PerlinNoise2D_MEM::skipObjectCount();
				discards--;
				std::cout <<"Skip Object initialization." << "\n";
				continue;
			}
			if (amount > 0)
			{
				uint32_t size_x = TOTAL_SIZE_X / SUBSECTIONS_X * params.freq_x + 1;
				uint32_t size_y = TOTAL_SIZE_Y / SUBSECTIONS_Y * params.freq_y + 1;

				if (SUBSECTIONS_X == 1)
					size_x--;
				if (SUBSECTIONS_Y == 1)
					size_y--;

				mapParts.push_back(new PerlinNoise2D_MEM(size_x, size_y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED));
				amount--;
				continue;
			}

			/*mapParts.push_back(NULL);
			PerlinNoise2D_MEM::skipObjectCount();
			std::cout << "Skip Object initialization." << "\n";
			continue;
			*/
			break;
		}

		worker = new std::thread[mapParts.size()];

	};
	void deconstructMapParts()
	{
		mapParts.clear();
		delete worker;
	};
	void threadWorker();
	void calcMapPart(uint32_t i);

private:

	PerlinParameters params;
	std::vector<PerlinNoise2D_MEM*> mapParts;
	const uint32_t TOTAL_SIZE_X, TOTAL_SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED;
	std::thread* worker;

};

