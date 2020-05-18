#include "PerlinNoise.h"
#include <thread>

int PerlinNoise2D_MEM::objectCount{ 0 };
void PerlinNoise2D::createPPMFile(std::string filename, PerlinModifiers mod)
{
    uint32_t mapPartPos_x = (objectID - 1) % SUBSECTIONS_X;
    uint32_t mapPartPos_y = (int)((objectID - 1) / SUBSECTIONS_X);
    //uint32_t sz_x = mapPartPos_x == SUBSECTIONS_X - 1 ? SIZE_X / mod.scale_x : (SIZE_X-1) / mod.scale_x;
    //uint32_t sz_y = mapPartPos_y == SUBSECTIONS_Y - 1 ? SIZE_Y / mod.scale_y : (SIZE_Y-1) /mod.scale_x;
    uint32_t sz_x = SUBSECTIONS_X > 1 ? (SIZE_X - 1) / mod.scale_x : SIZE_X / mod.scale_x ;
    uint32_t sz_y = SUBSECTIONS_Y > 1 ? (SIZE_Y - 1) / mod.scale_y : SIZE_Y / mod.scale_y;
    std::cout << "Create .ppm file...\n";
    std::ofstream ofs;
    std::ofstream file;
    ofs.open("./Output/"+filename+".ppm", std::ios::out | std::ios::binary);
    file.open("./Output/" + filename + "_output.txt");
    file << "Calculations\n";
    ofs << "P6\n" << sz_x << " " << sz_y << "\n255\n";
    for (int i = 0; i < sz_y; ++i) {
        for (int j = 0; j < sz_x; ++j)
        {
            double perlinValue;
            if (mod.grayscales == 1)
                perlinValue = (noise(( (j + 0) * mod.scale_x), ((i + 0) * mod.scale_y)) + mod.height) * mod.amplitude;
            else
                perlinValue = (int)((noise(((j + 0) * mod.scale_x), ((i + 0) * mod.scale_y)) + mod.height) * mod.amplitude);
            unsigned char n = static_cast<unsigned char>(perlinValue * 255 / mod.grayscales);
            ofs << n << n << n;
            //file << "x"<<(j) * mod.scale_x << "y" << (i) * mod.scale_y << ":" << perlinValue << " ";
            
                int x0 = (int)(j * mod.scale_x);
                int y0 = (int)(i * mod.scale_y);
                int x1 = x0 + 1;
                int y1 = y0 + 1;
                double* x0y0 = getVector(x0, y0), * x1y0 = getVector(x1, y0), * x0y1 = getVector(x0, y1), * x1y1 = getVector(x1, y1);
                //std::cout << "BugAnalysis. x" << j * mod.scale_x << "y" << i * mod.scale_y << ": " << x0y0[0] << "/" << x1y0[0] << "/" << x0y1[0] << "/" << x1y1[0] << "=" << perlinValue;
                file << "x" << (j)*mod.scale_x << "y" << (i)*mod.scale_y << "v[x]" << x0y0[0] << "v[y]" << x0y0[1] <<"p[V]"  << perlinValue <<" ";
                delete x0y0;
                delete x1y0;
                delete x0y1;
                delete x1y1;
        }
        file << "\n";
    }
    ofs.close();
    std::cout << ".ppm file was successfully created.\n";
}


// offset_x und offset_y m�ssen entfernt werden.
//void PerlinNoise2D::drawNoise(unsigned int width, unsigned int height, PerlinModifiers mod)
//{
//    sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");
//    sf::Image perlinnoise;
//    perlinnoise.create(width, height);
//    
//    sf::Texture text;
//    text.loadFromImage(perlinnoise);
//    sf::Sprite sprite;
//    sprite.setTexture(text, true);
//
//    std::mt19937 random_mod(2000);
//    std::uniform_real_distribution<double> double_range(-0.05, 0.05);
//
//
//    while (window.isOpen())
//    {
//        
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        
//        
//        for (int i = 0; i < height;i++)
//        {
//            for (int j = 0; j < width;j++)
//            { 
//                //mod = double_range(random_mod);
//                double perlinValue;
//                if (mod.grayscales == 1)
//                {
//                    perlinValue = (noise(((j + offset_x)  * mod.scale_x) , ((i + offset_y) * mod.scale_y) ) + mod.height) * mod.amplitude;
//                }
//                else
//                {
//                    perlinValue = (int)((noise(((j + offset_x) * mod.scale_x) , ((i + offset_y) * mod.scale_y) ) + mod.height) * mod.amplitude);
//                }
//                perlinnoise.setPixel(j, i , sf::Color(perlinValue * mod.grayscales * 2, perlinValue* mod.grayscales * 2, perlinValue* mod.grayscales * 2));
//
//            }
//        }
//        text.loadFromImage(perlinnoise);
//        sprite.setTexture(text, true);
//        window.clear();
//        window.draw(sprite);
//        window.display();
//
//        //Parameter, die ver�ndert werden
//        //amp *= 0.999;
//        //mod.scale_x *= 0.99;
//        //magy *= 0.99;
//    }
//}

// Lineare Interpolation
inline double PerlinNoise2D::lerp(double lo, double hi, double t)
    {
        return lo * (1 - t) + hi * t;
    };

// Skalarprodukt
double PerlinNoise2D::dotProduct(double xi, double yi, double x, double y)
{
    return  xi * x + yi * y;
};

// Einheitsvektor
double* PerlinNoise2D::calcUnitVector(double x, double y, double angle)
{
    angle = angle * std::_Pi / 180;
    double* unitVector = new double[2]{ cos(angle),sin(angle) };
    return unitVector;
};


double PerlinNoise2D::smoothening(double num)
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

    

    // Gewichte f�r x und y, um Kurve zu gl�tten
    double sx = x - (double)x0;
    double sy = y - (double)y0;

    sx = smoothening(sx);
    sy = smoothening(sy);

    double n0, n1, ix0, ix1, value;

    double* x0y0 = getVector(x0, y0), *x1y0 = getVector(x1,y0), *x0y1 = getVector(x0,y1), *x1y1 = getVector(x1, y1);



    n0 = dotProduct(x0y0[0], x0y0[1], x - (double)x0, y - (double)y0);
    n1 = dotProduct(x1y0[0], x1y0[1], x - (double)x1, y - (double)y0);
    ix0 = lerp(n0, n1, sx);

    n0 = dotProduct(x0y1[0], x0y1[1], x - (double)x0, y - (double)y1);
    n1 = dotProduct(x1y1[0], x1y1[1], x - (double)x1, y - (double)y1);
    ix1 = lerp(n0, n1, sx);

    value = lerp(ix0, ix1, sy);
    
    delete x0y0;
    delete x1y0;
    delete x0y1;
    delete x1y1;

    return value;
};

std::string PerlinNoise2D::toString()
{

    std::string output = "SIZE_X: " + std::to_string(SIZE_X) + " // SIZE_Y: " + std::to_string(SIZE_Y) + 
        " // Subsections_X: " + std::to_string(SUBSECTIONS_X) + " // Subsections_Y: " + std::to_string(SUBSECTIONS_Y);
    return output;
};

double* PerlinNoise2D_FILE::getVector(uint32_t x0, uint32_t y0)
{
    //myfile.ignore
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
            double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
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
            double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
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

    //std::cout << "wholeMap_size_x: " << mapPartPos_x << " wholeMap_size_y: " << mapPartPos_y << "\n";


    //mt_rng.discard(SIZE_X);
    mt_rng.discard(mapPartPos_y * adjusted_size_x * adjusted_size_y * SUBSECTIONS_X);
    //std::cout << "Beginning Discards: " << (mapPartPos_y * wholeMap_size_x * SIZE_Y) << "\n";

    for (int k = 0; SIZE_Y > k; k++)
    {
        
        bool visited_j = false;
        for (int j = 0; SUBSECTIONS_X > j; j++)
        {
            //std::cout << "{" << objectID << "}LOOPBEGIN: j: " << j << " k:" << k <<"\n";
            if (j < mapPartPos_x)
            {
                mt_rng.discard(adjusted_size_x);
                //std::cout << "{" << objectID << "}: j<: " << adjusted_size_x << "\n";
            }

            if (j == mapPartPos_x)
            {
                visited_j = true;
                for (int l = 0; SIZE_X > l; l++)
                {
                    bool mapBorder_x_reached = mapPartPos_x == SUBSECTIONS_X - 1 && SIZE_X - 1 == l;
                    bool mapBorder_y_reached = mapPartPos_y == SUBSECTIONS_Y - 1 && SIZE_Y - 1 == k;
                    bool isMapPart_x = SUBSECTIONS_X != 1;
                    bool isMapPart_y = SUBSECTIONS_Y != 1;
                    
                    if (mapBorder_x_reached && mapBorder_y_reached && l == adjusted_size_x && k == adjusted_size_y)
                    {
                        std::mt19937 mt_border_xy(SEED);
                        double* vector = calcUnitVector(0.0, 0.0, float_range(mt_border_xy));
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
                        //std::cout << "{" << objectID << "}borderx reached. discards: "<< discards << "\n";
                        double* vector = calcUnitVector(0.0, 0.0, float_range(mt_border_x));
                        grid[l][k][0] = vector[0];
                        grid[l][k][1] = vector[1];
                        delete vector;
                        continue;
                    }
                    if (mapBorder_y_reached && isMapPart_y)
                    {
                        std::mt19937 mt_border_y(SEED);
                        uint32_t discards = (mapPartPos_x * adjusted_size_x)+l;
                        mt_border_y.discard(discards);
                        //std::cout << "{" << objectID << "}bordery reached. discards: " << discards << "\n";
                        double* vector = calcUnitVector(0.0, 0.0, float_range(mt_border_y));
                        grid[l][k][0] = vector[0];
                        grid[l][k][1] = vector[1];
                        delete vector;
                        continue;
                    }
                    double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
                    grid[l][k][0] = vector[0];
                    grid[l][k][1] = vector[1];
                    delete vector;

                }
                //std::cout << "{" << objectID << "}: j==: " << SIZE_X << "\n";
            }

            if (j > mapPartPos_x)
            {
                //std::cout << "{" << objectID << "}: j>: " << (visited_j ? adjusted_size_x - 1 : adjusted_size_x) << (visited_j ? " visited_j" : "!visited_j") << "\n";
                mt_rng.discard(visited_j ? SIZE_X - 2 : SIZE_X );
                visited_j = false;
            }

        }
        //std::cout << "\n\n";
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
            double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
            myfile << "x"<<j<<"y"<<i <<"="<< grid[j][i][0] << ";" << grid[j][i][1] << " | ";
        }
        myfile << "\n";
    }
    myfile.close();
};




//void PerlinNoise2D::threadTask(int i)
//{
//    std::cout << "Worker has been started. ID: "<< i << "\n";
//
//};
//
//
//void PerlinNoise2D::threadWorker()
//{
//    //std::thread worker[SUBSECTIONS_X][SUBSECTIONS_Y];
//    std::thread worker((&PerlinNoise2D::threadTask), this, 4);
//    //worker = std::thread(threadTask);
//    worker.join();
//};

//void PerlinNoise2D_MEM::createGridPart(uint32_t wholeMap_size_x, uint32_t wholeMap_size_y)
//{
//    uint32_t mapPartPos_x = (objectID - 1) % SUBSECTIONS_X;
//    uint32_t mapPartPos_y = (int)((objectID - 1) / SUBSECTIONS_X);
//    uint32_t adjusted_size_x = SUBSECTIONS_X - 1 == mapPartPos_x ? SIZE_X : SIZE_X - 1;
//    uint32_t adjusted_size_y = SUBSECTIONS_Y - 1 == mapPartPos_y ? SIZE_Y : SIZE_Y - 1;
//
//    std::cout << "wholeMap_size_x: " << mapPartPos_x << " wholeMap_size_y: " << mapPartPos_y << "\n";
//
//
//    //mt_rng.discard(SIZE_X);
//    mt_rng.discard(mapPartPos_y * adjusted_size_x * adjusted_size_y * SUBSECTIONS_X);
//    std::cout << "Beginning Discards: " << (mapPartPos_y * wholeMap_size_x * SIZE_Y) << "\n";
//
//    for (int k = 0; SIZE_Y > k; k++)
//    {
//
//        bool visited_j = false;
//        for (int j = 0; SUBSECTIONS_X > j; j++)
//        {
//            std::cout << "{" << objectID << "}LOOPBEGIN: j: " << j << " k:" << k << "\n";
//            if (j < mapPartPos_x)
//            {
//                mt_rng.discard(adjusted_size_x);
//                std::cout << "{" << objectID << "}: j<: " << adjusted_size_x << "\n";
//            }
//
//            if (j == mapPartPos_x)
//            {
//                visited_j = true;
//                for (int l = 0; SIZE_X > l; l++)
//                {
//                    bool mapBorder_x_reached = mapPartPos_x == SUBSECTIONS_X - 1 && SIZE_X - 1 == l;
//                    if (mapBorder_x_reached)
//                    {
//                        grid
//                    }
//                    double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
//                    grid[l][k][0] = vector[0];
//                    grid[l][k][1] = vector[1];
//                    delete vector;
//
//                }
//                std::cout << "{" << objectID << "}: j==: " << SIZE_X << "\n";
//            }
//
//            if (j > mapPartPos_x)
//            {
//                std::cout << "{" << objectID << "}: j>: " << (visited_j ? adjusted_size_x - 1 : adjusted_size_x) << (visited_j ? " visited_j" : "!visited_j") << "\n";
//                mt_rng.discard(visited_j ? SIZE_X - 2 : SIZE_X);
//                visited_j = false;
//            }
//
//        }
//        //std::cout << "\n\n";
//    }
//};