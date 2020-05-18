#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include "PerlinNoise.h"
#include "PerlinModifiers.h"
#include <SFML/Graphics.hpp>
#include "PerlinMap.h"
#include "PerlinMap.cpp" // Zur Vermeidung von Linker Problemen bei Verwendung von Templates!
#include <vector>


int main(int argc, char* argv[])
{
    uint32_t SIZE_X = 64, SIZE_Y = 64;
    uint32_t SUBSECTIONS_X = 2, SUBSECTIONS_Y = 2;
    uint32_t SEED = 2020;
    uint32_t discards = 0;
    uint32_t amount = SUBSECTIONS_X * SUBSECTIONS_Y;
    discards = 2;
    amount = 1;

    for (int i = 0; i < argc; i++)
    {
        std::string input = argv[i];
        if (input.compare("-x") == 0 || input.compare("--size_x") == 0 )
        {
            SIZE_X = std::atoi(argv[i + 1]);
        }

        if (input.compare("-y") == 0 || input.compare("--size_y") == 0 )
        {
            SIZE_Y = std::atoi(argv[i + 1]);
        }

        if (input.compare("-sx") == 0 || input.compare("--subsections_x") == 0 )
        {
            SUBSECTIONS_X = std::atoi(argv[i + 1]);
        }

        if (input.compare("-sy") == 0 || input.compare("--subsections_y") == 0 )
        {
            SUBSECTIONS_Y = std::atoi(argv[i + 1]);
        }

        if (input.compare("-s") == 0 || input.compare("--seed") == 0 )
        {
            SEED = std::atoi(argv[i + 1]);
        }

        if (input.compare("-s") == 0 || input.compare("--seed") == 0 )
        {
            SEED = std::atoi(argv[i + 1]);
        }

        if (input.compare("-d") == 0 || input.compare("--discard") == 0)
        {
            discards = std::atoi(argv[i + 1]);
        }

        if (input.compare("-a") == 0 || input.compare("--amount") == 0)
        {
            amount = std::atoi(argv[i + 1]);
        }


    }
    
    if (argc > 1) std::cout << "SIZE_X: " << SIZE_X << " SIZE_Y: " << SIZE_Y << " SUBSECTIONS_X: " << SUBSECTIONS_X << " SUBSECTIONS_Y: " << SUBSECTIONS_Y << " SEED: " << SEED << "\n";
    
    
    //Perlin Noise Parameter



    PerlinMap<PerlinNoise2D_MEM> map(SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, discards, amount, SEED);
    //map.threadWorker();
    map.calcMap();
    //PerlinNoise2D_MEM noise(SIZE_X, SIZE_Y, 0, 0, SEED);
    //noise.threadWorker();
    //std::thread worker(test, 4);

    //worker.join();

	return 0;
}