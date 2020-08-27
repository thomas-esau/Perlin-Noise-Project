#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include "PerlinNoise.h"
#include "PerlinParameters.h"
#include "PerlinMap.h"
#include <vector>


int main(int argc, char* argv[])
{
    // Standardwerte für die Erzeugung des PerlinMap Objekts, falls keine Command Line Argumente übergeben wurden.
    uint32_t TOTAL_SIZE_X = 300, TOTAL_SIZE_Y = 300;
    uint32_t SUBSECTIONS_X = 3, SUBSECTIONS_Y = 3;
    uint32_t SEED = 2016;
    uint32_t discards = 0;
    uint32_t amount = SUBSECTIONS_X * SUBSECTIONS_Y;


    for (int i = 0; i < argc; i++)
    {
        std::string input = argv[i];
        if (input.compare("-x") == 0 || input.compare("--size_x") == 0 )
        {
            TOTAL_SIZE_X = std::atoi(argv[i + 1]);
        }

        if (input.compare("-y") == 0 || input.compare("--size_y") == 0 )
        {
            TOTAL_SIZE_Y = std::atoi(argv[i + 1]);
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

        if (input.compare("-d") == 0 || input.compare("--discard") == 0)
        {
            discards = std::atoi(argv[i + 1]);
        }

        if (input.compare("-a") == 0 || input.compare("--amount") == 0)
        {
            amount = std::atoi(argv[i + 1]);
        }


    }
    
    if (argc > 1)
    {
        std::cout << "TOTAL_SIZE_X: " << TOTAL_SIZE_X << " TOTAL_SIZE_Y: " << TOTAL_SIZE_Y << " SUBSECTIONS_X: " << 
            SUBSECTIONS_X << " SUBSECTIONS_Y: " << SUBSECTIONS_Y << " SEED: " << SEED << "\n";
    }

    PerlinMap map(TOTAL_SIZE_X, TOTAL_SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, discards, amount, SEED);
    map.threadWorker();
	return 0;
}