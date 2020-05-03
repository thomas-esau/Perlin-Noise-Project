#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include "PerlinNoise.h"
#include <SFML/Graphics.hpp>


int main()
{

    // Perlin Noise Parameter
    double magx = 1/2;
    double magy = 1/2;
    double height = 1;
    double amp = 0.5 * 8;
    int grayscales = amp * 2;

    const uint32_t XPOS = 400, YPOS = 400;
    const uint32_t OFFSET_X = 0, OFFSET_Y = 0;
    const uint32_t SEED = 2020;

    PerlinNoise2D_MEM noise(XPOS, YPOS, OFFSET_X, OFFSET_Y, SEED);

    //noise.createGrid();
    noise.createPPMFile("./Output/Ges.ppm", magx, magy, height, amp, grayscales);
    
    /*
    const uint32_t x_parts = 2, y_parts = 2;
    PerlinNoise2D_MEM parts[x_parts][y_parts];
    for (int i = 0; i < y_parts; i++)
    {
        for (int j = 0; j < x_parts; j++)
        {
            parts[j][i].resetPerlinNoise2D(XPOS / x_parts, YPOS / y_parts, OFFSET_X / x_parts * (j + 1), OFFSET_Y / y_parts * (i + 1), SEED);
        }
    }
    */
    noise.drawNoise(XPOS, YPOS, magx, magy, height, amp, grayscales);

    

	return 0;
}