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

void test(int challo)
{
    std::cout << "Thread Created: " << std::this_thread::get_id() << " Nummer: " << challo << "\n";
}

int main()
{

    // Perlin Noise Parameter

    const uint32_t SIZE_X = 16, SIZE_Y = 16;
    const uint32_t SUBSECTIONS_X = 2, SUBSECTIONS_Y = 2;
    const uint32_t SEED = 2020;
    PerlinModifiers mod;

    PerlinMap<PerlinNoise2D_MEM> map(SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED);
    //map.threadWorker();
    map.calcMap();
    //PerlinNoise2D_MEM noise(SIZE_X, SIZE_Y, 0, 0, SEED);
    //noise.threadWorker();
    //std::thread worker(test, 4);

    //worker.join();

	return 0;
}