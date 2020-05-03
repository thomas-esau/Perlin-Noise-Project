#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include "PerlinNoise.h"
#include <SFML/Graphics.hpp>


int main()
{

    // Perlin Noise Parameter
    double magx = 1./8;
    double magy = 1./8;
    double height = 1;
    double amp = 0.5 * 8;
    int grayscales = amp * 2;

    const uint32_t XPOS = 256, YPOS = 256;
    const uint32_t OFFSET_X = 0, OFFSET_Y = 0;
    const uint32_t SEED = 2020;

    PerlinNoise2D_MEM noise(XPOS*magx, YPOS*magy, OFFSET_X, OFFSET_Y, SEED);

    noise.createPPMFile("./Output/Ges.ppm", magx, magy, height, amp, grayscales);
    
    
    const uint32_t x_parts = 2, y_parts = 2;
    PerlinNoise2D_MEM parts[x_parts][y_parts];
    for (int i = 0; i < y_parts; i++)
    {
        for (int j = 0; j < x_parts; j++)
        {
            parts[j][i].resetPerlinNoise2D(XPOS / x_parts * magx, YPOS / y_parts * magy, x_parts * (j + 1), y_parts * (i + 1), SEED);
            std::string path = "./Output/x";
            path.append(std::to_string(j)+"y"+ std::to_string(i));
            
            noise.createPPMFile(path+".ppm", magx, magy, height, amp, grayscales);
        }
    }
    
    //noise.drawNoise(XPOS, YPOS, magx, magy, height, amp, grayscales);
    //noise2.drawNoise(XPOS, YPOS, magx, magy, height, amp, grayscales);

    

	return 0;
}