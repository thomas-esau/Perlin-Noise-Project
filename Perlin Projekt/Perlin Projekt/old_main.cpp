//PerlinNoise2D_MEM noise(SIZE_X, SIZE_Y, SEED);
//PerlinNoise2D_FILE file(SIZE_X, SIZE_Y, SEED);

//PerlinNoise2D *noise2 = new PerlinNoise2D_MEM(SIZE_X, SIZE_Y, SEED, mod);


//map.noise->createPPMFile("./Output/Test.ppm");

    //noise.createPPMFile("./Output/Ges.ppm");


    /*
    const uint32_t x_parts = 2, y_parts = 2;
    PerlinNoise2D_MEM parts[x_parts][y_parts];
    for (int i = 0; i < y_parts; i++)
    {
        for (int j = 0; j < x_parts; j++)
        {
            parts[j][i].resetPerlinNoise2D(SIZE_X / x_parts * magx, SIZE_Y / y_parts * magy, x_parts * (j + 1), y_parts * (i + 1), SEED);
            std::string path = "./Output/x";
            path.append(std::to_string(j)+"y"+ std::to_string(i));

            noise.createPPMFile(path+".ppm");
        }
    }
    */

    //noise.drawNoise(SIZE_X, SIZE_Y);

    //noise2.drawNoise(SIZE_X, SIZE_Y, magx, magy, height, amp, grayscales);


/*
void constructMapParts()
{
    mapPart = new T * [SIZE_X];
    for (int i = 0; i < SIZE_Y; i++)
    {
        mapPart[i] = new T[SIZE_Y];
    }
};
*/