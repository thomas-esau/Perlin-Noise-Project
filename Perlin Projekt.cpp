// Perlin Projekt.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
constexpr auto XPOS = 128;
constexpr auto YPOS = 128;


// Random Number Generator, initialisierung mit seed
const unsigned int seed = 2020;
std::mt19937 mt_rng(seed);
std::uniform_real_distribution<float> float_range(0, 360);
std::uniform_real_distribution<float> zonetoone(-1, 1);

// Lineare Interpolation
inline double lerp(double lo, double hi, double t)
{
    return lo * (1 - t) + hi * t;
};

// Skalarprodukt
double dotProduct(double xi, double yi, double x, double y)
{

    return  xi * x + yi * y;
};

// Einheitsvektor
double* calcUnitVector(double x, double y, double angle)
{
    angle = angle * std::_Pi / 180;
    //double decPoints = 0.000001;

    //double unitVector[2]{ (int)(cos(angle) / decPoints) * decPoints,(int)(sin(angle) / decPoints) * decPoints };
    double unitVector[2]{ cos(angle),sin(angle)};
    return unitVector;
}

void fillGrid(double (&grid)[XPOS][YPOS][2])
{
    std::cout << "Fülle Grid: " << "\n";
    // Vektorgitter wird mit Gradienten gefüllt
    for (int i = 0; YPOS > i; i++)
    {
        for (int j = 0; XPOS > j; j++)
        {
            double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
            grid[j][i][0] = vector[0];
            grid[j][i][1] = vector[1];
            //std::cout << "xpos: " << j << " ypos: " << i << " xval: " << vector[0] << " yval : " << vector[1] << "\n";
        }
    }
}

double smoothening(double num)
{
    return num * num * (3 - 2 * num);
    //return num;
}


double perlin1d(double x, double const (&arr)[XPOS])
{
    int x0 = (int)x;
    int x1 = x0 + 1;
    x = x - x0;
    double sx = smoothening(x);
    double ix0 = lerp(arr[x0], arr[x1], sx);
    return ix0;
}

double perlin2d(double x, double y, double const (&grid)[XPOS][YPOS][2])
{

    // Zelle, in der sich x und y befinden
    int x0 = (int) x;
    int x1 = x0 + 1;
    int y0 = (int) y;
    int y1 = y0 + 1;

    // x und y auf Koordinaten innerhalb der Zelle beziehen
 
    //std::cout << "x0: " << x0 << " , x1: " << x1 << " , y0: " << y0 << " , y1: " << y1 << " --- x: " << x << " , y: " << y << "\n";

    // Gewichte für x und y, um Kurve zu glätten

    double sx = x - (double)x0;
    double sy = y - (double)y0;

    sx = smoothening(sx);
    sy = smoothening(sy);

    double n0, n1, ix0, ix1, value;

    n0 = dotProduct(grid[x0][y0][0], grid[x0][y0][1], x - (double)x0, y - (double)y0);
    n1 = dotProduct(grid[x1][y0][0], grid[x1][y0][1], x - (double)x1, y - (double)y0);
    ix0 = lerp(n0, n1, sx);


    n0 = dotProduct(grid[x0][y1][0], grid[x0][y1][1], x - (double)x0, y - (double)y1);
    n1 = dotProduct(grid[x1][y1][0], grid[x1][y1][1], x - (double)x1, y - (double)y1);
    ix1 = lerp(n0, n1, sx);

    value = lerp(ix0, ix1, sy);
    return value;
};

void createHeightMap()
{


};


int main()
{
    double grid[XPOS][YPOS][2]{};
    fillGrid(grid);

    /*
    double grid[XPOS][YPOS][2]{};
    fillGrid(grid);
    std::ofstream myfile;
    myfile.open("example4.txt");
    std::cout << "Hello World!\n";
    for (double x = 0; x < 10; x += 0.001)
    {
        myfile << x << "    " << perlin2d(x, x, grid) *0.1 << "\n";
    }
    myfile.close();
    */

    
    /*
    std::ofstream myfile;
    myfile.open("example4.txt");
    std::cout << "Hello World!\n";
    for (double x = 0; x < 10; x += 0.001)
    {
        myfile << x << "    " << perlin2d(x, 0, grid) * 0.1 << "\n";
    }
    myfile.close();
    */

    /*
    std::ofstream myfile;
    myfile.open("example3.txt");
    double arr[XPOS]{};
    for (int i = 0; i < XPOS; i++)
        arr[i] = zonetoone(mt_rng);

    for (float i = 0; i < XPOS - 1; i += 0.1)
        //std::cout << perlin1d(i, arr) << "\n";
        myfile << i << "    " << perlin1d(i,arr) << "\n";
    myfile.close();
    */



    
    double perlinvalues[XPOS][YPOS];
    // Magnify
    double magx = 0.05;
    double magy = 0.05;
    double height = 1;
    double amp = 0.5;

    for (int i = 0; YPOS > i; i++)
    {
        for (int j = 0; XPOS > j; j++)
        {
            perlinvalues[j][i] = (perlin2d((j * magx), (i * magy), grid) + height) * amp;
        }
    }
    std::ofstream ofs;
    ofs.open("./perlin.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << XPOS << " " << YPOS << "\n255\n";
    for (int i = 0; i < YPOS; ++i) {
        for (int j = 0; j < XPOS; ++j)
        {
            unsigned char n = static_cast<unsigned char>(perlinvalues[j][i] * 255);
            //std::cout << perlinvalues[j][i] << "\n";
            //std::cout << n << n;
            ofs << n << n << n;
        }
    }
    ofs.close();
    


    /*
    float perlinvalues[XPOS * YPOS];

    for (int i = 0; YPOS > i; i++)
    {
        for (int j = 0; XPOS> j; j++)
        {
            perlinvalues[i * XPOS + j] = (perlin2d((j * 1/8. ), (i * 1 / 8.), grid) + 1) * 0.5;
        }
    }
    std::ofstream ofs;
    ofs.open("./noise2.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << XPOS << " " << YPOS << "\n255\n";
        for (int k = 0; k < XPOS * YPOS; ++k)
        {
            unsigned char n = static_cast<unsigned char>(perlinvalues[k] * 255);
            std::cout << n;
            ofs << n << n << n;
        }
    ofs.close();
    */

    /*
    for (int i = 0; i < 100; i++)
    {
        std::cout << int_range(mt_rng) / 100.0 << "\n";
    }
    */
    //perlin(1.2, 1.1, grid);


}
