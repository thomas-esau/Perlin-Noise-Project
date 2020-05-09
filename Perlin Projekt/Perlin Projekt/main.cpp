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

int main()
{

    // Perlin Noise Parameter

    const uint32_t SIZE_X = 1024, SIZE_Y = 1024;
    const uint32_t SUBSECTIONS_X = 2, SUBSECTIONS_Y = 2;
    const uint32_t SEED = 2020;
    PerlinModifiers mod;
    mod.scale_x = 0.05;
    mod.scale_y = 0.05;
    mod.amplitude = 2;
    mod.grayscales = 1;
    mod.height = 1;



    PerlinMap<PerlinNoise2D_MEM> map(SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED);
    map.calcMap();
    //PerlinNoise2D_MEM noise(SIZE_X, SIZE_Y, 0, 0, SEED);

    std::mt19937 mt_rng(SEED);
    std::mt19937 mt_rng2(SEED);
    std::uniform_int_distribution<int> range(1, 100);
    
    /*
    mt_rng.discard(0);
    for (int i = 0; i < 10; i++)
    std::cout << range(mt_rng) << " ";

    std::cout << "\n";
    
    for (int i = 0; i < 10; i++)
    std::cout << range(mt_rng2) << " ";
    */

    
    

	return 0;
}