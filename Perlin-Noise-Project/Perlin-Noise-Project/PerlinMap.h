#pragma once
#include <cstdint>
#include "PerlinNoise.h"
#include <vector>
#include <thread>

template <typename T>
class PerlinMap
{
public:

	PerlinMap(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SUBSECTIONS_X, uint32_t init_SUBSECTIONS_Y, uint32_t discards, uint32_t amount , uint32_t init_SEED) :
		TOTAL_SIZE_X{ init_SIZE_X }, TOTAL_SIZE_Y{ init_SIZE_Y }, SUBSECTIONS_X{ init_SUBSECTIONS_X }, SUBSECTIONS_Y{ init_SUBSECTIONS_Y }, SEED{ init_SEED }
	{
		//PerlinParameters
		params.freq_x = 1/32.;
		params.freq_y = 1/32.;
		params.amplitude = 2;
		params.height = 1;

		uint32_t sub_size_x = TOTAL_SIZE_X / SUBSECTIONS_X * params.freq_x;
		uint32_t sub_size_y = TOTAL_SIZE_Y / SUBSECTIONS_Y * params.freq_y;

		constructMapParts(discards, amount, sub_size_x, sub_size_y);

		worker = new std::thread[SUBSECTIONS_X * SUBSECTIONS_Y];

	};

	void calcMap();
	bool isSplittedMap() { return SUBSECTIONS_X == 1 && SUBSECTIONS_Y == 1 ? false : true; };
	void setParams(PerlinParameters setParams) { params = setParams; };
	void constructMapParts(uint32_t discards, uint32_t amount, uint32_t sub_size_x, uint32_t sub_size_y)
	{
		for (int i = 0; i < SUBSECTIONS_Y; i++)
		{
			std::vector<T*> row;
			for (int j = 0; j < SUBSECTIONS_X; j++)
			{
				if (discards > 0)
				{
					row.push_back(NULL);
					T::skipObjectCount();
					discards--;
					std::cout <<"Skip Object initialization." << "\n";
					continue;
				}
				if (amount > 0)
				{
					uint32_t mapPart_size_x = sub_size_x + 1;
					uint32_t mapPart_size_y = sub_size_y + 1;

					if (SUBSECTIONS_X == 1)
						mapPart_size_x--;
					if (SUBSECTIONS_Y == 1)
						mapPart_size_y--;

					row.push_back(new T(mapPart_size_x, mapPart_size_y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED));
					amount--;
					
					std::string filename = "x" + std::to_string(j) + "y" + std::to_string(i);
					std::cout << row.at(j)->toString() << "\n";
					continue;
				}

				row.push_back(NULL);
				T::skipObjectCount();

				std::cout << "Skip Object initialization." << "\n";
				continue;

			}
			mapParts.push_back(row);
		}

	};
	void deconstructMapParts()
	{
		mapParts.clear();
	};
	void threadWorker();
	void threadTask(uint32_t i);

private:

	PerlinParameters params;
	std::vector<std::vector<T*>> mapParts;
	const uint32_t TOTAL_SIZE_X, TOTAL_SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED;
	std::thread* worker;

};

