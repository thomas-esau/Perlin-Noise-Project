#include "PerlinNoise.h"
#include <thread>

int PerlinNoise2D_MEM::objectCount{ 0 };
void PerlinNoise2D::createFile(std::string filename, std::string filetype, PerlinParameters params)
{
    uint32_t adjusted_size_x = SUBSECTIONS_X > 1 ? (SIZE_X - 1) / params.freq_x : SIZE_X / params.freq_x;
    uint32_t adjusted_size_y = SUBSECTIONS_Y > 1 ? (SIZE_Y - 1) / params.freq_y : SIZE_Y / params.freq_y;

    if (filetype.compare("ppm") != 0 && filetype.compare("obj") != 0)
    {
        std::cout << "Filetype not supported. Try ppm or obj." << "\n";
    }

    std::cout << "("<< objectID << ") Create ."+filetype+"file...\n";
    std::ofstream fileoutput;

    if (filetype.compare("ppm") == 0)
    {
        fileoutput.open("./Output/" + filename + "." + filetype, std::ios::out | std::ios::binary);
        fileoutput << "P6\n" << adjusted_size_x << " " << adjusted_size_y << "\n255\n";
        for (int i = 0; i < adjusted_size_y; ++i) {
            for (int j = 0; j < adjusted_size_x; ++j)
            {
                double perlinValue;
                    perlinValue = (noise(((j + 0) * params.freq_x), ((i + 0) * params.freq_y)) + params.height) * params.amplitude;
                unsigned char n = static_cast<unsigned char>(perlinValue * 255);
                fileoutput << n << n << n;

            }
        }

    }

    if (filetype.compare("obj") == 0)
    {
        fileoutput.open("./Output/" + filename + "." + filetype);
        //v:
        for (int i = 0; i < adjusted_size_y + 1; ++i) {
            for (int j = 0; j < adjusted_size_x + 1; ++j) {
                double x = j * params.freq_x;
                double y = i * params.freq_y;
                double z = noise(x, y) * params.amplitude;
                fileoutput << "v " << x << " " << z << " " << y << "\n";
            }
        }

        //face:
        for (int i = 0; i < adjusted_size_y; ++i) {
            for (int j = 0; j < adjusted_size_x; ++j) {

                fileoutput << "f ";
                int x0y0 = i * (adjusted_size_x + 1) + j + 1;
                fileoutput << x0y0 << "/" << x0y0 << "/" << x0y0 << " ";
                int x1y0 = i * (adjusted_size_x + 1) + j + 2;
                fileoutput << x1y0 << "/" << x1y0 << "/" << x1y0 << " ";
                int x1y1 = (i + 1) * (adjusted_size_x + 1) + j + 2;
                fileoutput << x1y1 << "/" << x1y1 << "/" << x1y1 << " ";
                int x0y1 = (i + 1) * (adjusted_size_x + 1) + j + 1;
                fileoutput << x0y1 << "/" << x0y1 << "/" << x0y1 << "\n";
            }
        }
    }

    fileoutput.close();
    std::cout << "."+filetype+" file was successfully created.\n";
}

// Lineare Interpolation
inline double PerlinNoise2D::lerp(double lo, double hi, double w)
    {
        return lo * (1 - w) + hi * w;
    };

// Skalarprodukt
double PerlinNoise2D::dotProduct(double xi, double yi, double x, double y)
{
    return  xi * x + yi * y;
};

// Einheitsvektor
double* PerlinNoise2D::calcUnitVector(double angle)
{
    angle = angle * std::_Pi / 180;
    double* unitVector = new double[2]{ cos(angle),sin(angle) };
    return unitVector;
};


double PerlinNoise2D::smoothstep(double num)
{
    return num * num * (3 - 2 * num);
    //return num;
};

double PerlinNoise2D::noise(double x, double y)
{

    // Zelle, in der sich x und y befinden
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double n0, n1, n2, n3, ix0, ix1, value;

    double *x0y0 = getVector(x0, y0), * x1y0 = getVector(x1, y0), * x0y1 = getVector(x0, y1), * x1y1 = getVector(x1, y1);

    // Gewichte
    double wx = x - (double) x0;
    double wy = y - (double) y0;

    // Glättung der Gewichte
    double sx = smoothstep(wx);
    double sy = smoothstep(wy);

    n0 = dotProduct(x0y0[0], x0y0[1], x - (double)x0, y - (double)y0);
    n1 = dotProduct(x1y0[0], x1y0[1], x - (double)x1, y - (double)y0);
    n2 = dotProduct(x0y1[0], x0y1[1], x - (double)x0, y - (double)y1);
    n3 = dotProduct(x1y1[0], x1y1[1], x - (double)x1, y - (double)y1);

    ix0 = lerp(n0, n1, sx);
    ix1 = lerp(n2, n3, sx);
    value = lerp(ix0, ix1, sy);
    
    delete x0y0;
    delete x1y0;
    delete x0y1;
    delete x1y1;

    return value;
};


std::string PerlinNoise2D::toString()
{

    std::string output = "TOTAL_SIZE_X: " + std::to_string(SIZE_X) + " // TOTAL_SIZE_Y: " + std::to_string(SIZE_Y) + 
        " // Subsections_X: " + std::to_string(SUBSECTIONS_X) + " // Subsections_Y: " + std::to_string(SUBSECTIONS_Y);
    return output;
};

double* PerlinNoise2D_FILE::getVector(uint32_t x0, uint32_t y0)
{
    double* vector = new double[2];
    std::ifstream myfile;
    myfile.open("values.txt");
    std::string myText;

    unsigned int i = 0;
    while (myfile >> myText)
    {
        if (i == (SIZE_X * y0 * 2) + x0 * 2)
        {
            //std::cout << myText << "\n";
            vector[0] = std::stod(myText.c_str());
            //std::cout << vector[0] << "\n";
        }

        if (i == (SIZE_X * y0 * 2) + x0 * 2 + 1)
        {
            //std::cout << myText << "\n";
            vector[1] = std::stod(myText.c_str());
            //std::cout << vector[1] << "\n";
            //break;
        }
        i++;
    }
    myfile.close();

    return vector;

};

void PerlinNoise2D_FILE::createGrid()
{
    std::cout << "Create grid (FILE)..." << "\n";
	std::ofstream myfile;
	myfile.open("values.txt");
	
	for (int i = 0; i < SIZE_Y; i++)
	{
        for (int j = 0; j < SIZE_X; j++)
        {
            double* vector = calcUnitVector(float_range(mt_rng));
            myfile << vector[0] << " " << vector[1] << "\n";
        }
	}
	myfile.close();
    std::cout << "Grid (FILE) successfully created.\n";
}

double* PerlinNoise2D_MEM::getVector(uint32_t x0, uint32_t y0)
{
    if (x0 >= SIZE_X) x0 = x0 % SIZE_X;
    if (y0 >= SIZE_Y) y0 = y0 % SIZE_Y;
    double* vector = new double[2]{ grid[x0][y0][0], grid[x0][y0][1] };
    return vector;
};

void PerlinNoise2D_MEM::createGrid()
{
    for (int i = 0; SIZE_Y > i; i++)
    {
        for (int j = 0; SIZE_X > j; j++)
        {
            double* vector = calcUnitVector(float_range(mt_rng));
            grid[j][i][0] = vector[0];
            grid[j][i][1] = vector[1];
            delete vector;
        }
    }
};


void PerlinNoise2D_MEM::createGridPart(uint32_t wholeMap_size_x, uint32_t wholeMap_size_y)
{
    uint32_t mapPartPos_x = (objectID - 1) % SUBSECTIONS_X;
    uint32_t mapPartPos_y = (int)((objectID - 1) / SUBSECTIONS_X);
    uint32_t adjusted_size_x = SUBSECTIONS_X > 1 ? SIZE_X - 1 : SIZE_X;
    uint32_t adjusted_size_y = SUBSECTIONS_Y > 1 ? SIZE_Y - 1 : SIZE_Y;

    mt_rng.discard(mapPartPos_y * adjusted_size_x * adjusted_size_y * SUBSECTIONS_X);

    for (int k = 0; SIZE_Y > k; k++)
    {
        
        bool visitedPos_x = false;
        for (int j = 0; SUBSECTIONS_X > j; j++)
        {
            if (j < mapPartPos_x)
            {
                mt_rng.discard(adjusted_size_x);
            }

            if (j == mapPartPos_x)
            {
                visitedPos_x = true;
                for (int l = 0; SIZE_X > l; l++)
                {
                    bool mapBorder_x_reached = mapPartPos_x == SUBSECTIONS_X - 1 && adjusted_size_x == l;
                    bool mapBorder_y_reached = mapPartPos_y == SUBSECTIONS_Y - 1 && adjusted_size_y == k;
                    bool isMapPart_x = SUBSECTIONS_X != 1;
                    bool isMapPart_y = SUBSECTIONS_Y != 1;
                    
                    //if (mapBorder_x_reached && mapBorder_y_reached && l == adjusted_size_x && k == adjusted_size_y)
                    if (mapBorder_x_reached && mapBorder_y_reached && isMapPart_x && isMapPart_y)
                    {
                        std::mt19937 mt_border_xy(SEED);
                        double* vector = calcUnitVector(float_range(mt_border_xy));
                        grid[l][k][0] = vector[0];
                        grid[l][k][1] = vector[1];
                        delete vector;
                        continue;
                    }
                    
                    if (mapBorder_x_reached && isMapPart_x)
                    {
                        std::mt19937 mt_border_x(SEED);
                        uint32_t discards = mapPartPos_y * adjusted_size_x * adjusted_size_y * SUBSECTIONS_X + k * adjusted_size_x * SUBSECTIONS_X;
                        mt_border_x.discard(discards);
                        double* vector = calcUnitVector(float_range(mt_border_x));
                        grid[l][k][0] = vector[0];
                        grid[l][k][1] = vector[1];
                        delete vector;
                        continue;
                    }

                    if (mapBorder_y_reached && isMapPart_y)
                    {
                        std::mt19937 mt_border_y(SEED);
                        uint32_t discards = mapPartPos_x * adjusted_size_x + l;
                        mt_border_y.discard(discards);
                        double* vector = calcUnitVector(float_range(mt_border_y));
                        grid[l][k][0] = vector[0];
                        grid[l][k][1] = vector[1];
                        delete vector;
                        continue;
                    }
                    double* vector = calcUnitVector(float_range(mt_rng));
                    grid[l][k][0] = vector[0];
                    grid[l][k][1] = vector[1];
                    delete vector;

                }
            }

            if (j > mapPartPos_x)
            {
                //mt_rng.discard(visitedPos_x ? SIZE_X - 2 : SIZE_X );
                mt_rng.discard(visitedPos_x ? adjusted_size_x - 1 : adjusted_size_x);
                visitedPos_x = false;
            }

        }

    }
};

std::string PerlinNoise2D_MEM::toString()
{
    std::string output = "ID: " + std::to_string(objectID) + " " + PerlinNoise2D::toString();
    return output;
};

void PerlinNoise2D_MEM::dumpGrid()
{

    std::ofstream myfile;
    myfile.open("./Output/"+std::to_string(objectID)+".txt");

    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++)
        {
            double* vector = calcUnitVector(float_range(mt_rng));
            myfile << "x"<<j<<"y"<<i <<"="<< grid[j][i][0] << ";" << grid[j][i][1] << " | ";
        }
        myfile << "\n";
    }
    myfile.close();
};



void PerlinNoise2D_MEM::createGridPartOld(uint32_t x, uint32_t y)
{
    uint32_t mapPartPos_x = (objectID - 1) % SUBSECTIONS_X;
    uint32_t mapPartPos_y = (int)((objectID - 1) / SUBSECTIONS_X);

    std::cout << "x: " << mapPartPos_x << " y: " << mapPartPos_y << "\n";



    for (int i = 0; SUBSECTIONS_Y > i; i++)
    {
        mt_rng.discard(mapPartPos_y * x * SIZE_Y);
        for (int j = 0; SUBSECTIONS_X > j; j++)
        {
            for (int k = 0; SIZE_Y > k; k++)
            {
                if (j < mapPartPos_x) mt_rng.discard(SIZE_X);

                if (j == mapPartPos_x)
                {
                    for (int l = 0; SIZE_X > l; l++)
                    {
                        double* vector = calcUnitVector(float_range(mt_rng));
                        grid[l][k][0] = vector[0];
                        grid[l][k][1] = vector[1];
                        delete vector;
                    }
                }
                if (j > mapPartPos_x && i == mapPartPos_y) mt_rng.discard(SIZE_X);
            }
        }
        if (i == mapPartPos_y) return;
    }

};