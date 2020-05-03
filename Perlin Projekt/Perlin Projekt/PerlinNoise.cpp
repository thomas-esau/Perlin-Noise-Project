#include "PerlinNoise.h"
void PerlinNoise2D::createPPMFile(std::string filename, double magx, double magy, double height, double amp, int grayscales)
{
    std::cout << "Create .ppm file...\n";
    std::ofstream ofs;
    ofs.open("./"+filename, std::ios::out | std::ios::binary);
    ofs << "P6\n" << XPOS << " " << YPOS << "\n255\n";
    for (int i = 0; i < YPOS; ++i) {
        for (int j = 0; j < XPOS; ++j)
        {

            double perlinValue;
            if (grayscales == 1)
                perlinValue = (noise(( (j + OFFSET_X) * magx), ((i + OFFSET_Y) * magy)) + height) * amp;
            else
                perlinValue = (int)((noise(((j + OFFSET_X) * magx), ((i + OFFSET_Y) * magy)) + height) * amp);
            unsigned char n = static_cast<unsigned char>(perlinValue * 255 / grayscales);
            ofs << n << n << n;
        }
    }
    ofs.close();
    std::cout << ".ppm file was successfully created.\n";
}

void PerlinNoise2D::drawNoise(unsigned int width, unsigned int height, double magx, double magy, double height_p, double amp, int grayscales)
{
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");
    sf::Image perlinnoise;
    perlinnoise.create(width, height);
    
    sf::Texture text;
    text.loadFromImage(perlinnoise);
    sf::Sprite sprite;
    sprite.setTexture(text, true);

    std::mt19937 random_mod(2000);
    std::uniform_real_distribution<double> double_range(-0.05, 0.05);


    double mod = 0;
    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        
        
        for (int i = 0; i < height;i++)
        {
            for (int j = 0; j < width;j++)
            { 
                //mod = double_range(random_mod);
                double perlinValue;
                if (grayscales == 1)
                {
                    perlinValue = (noise(((j + OFFSET_X)  * magx) + mod, ((i + OFFSET_Y) * magy) + mod) + height_p) * amp;
                }
                else
                {
                    perlinValue = (int)((noise(((j + OFFSET_X) * magx) + mod, ((i + OFFSET_Y) * magy) + mod) + height_p) * amp);
                }
                perlinnoise.setPixel(j, i , sf::Color(perlinValue * grayscales * 2, perlinValue* grayscales * 2, perlinValue* grayscales * 2));

            }
        }
        text.loadFromImage(perlinnoise);
        sprite.setTexture(text, true);
        window.clear();
        window.draw(sprite);
        window.display();

        //Parameter, die verändert werden
        //amp *= 0.999;
        //magx *= 0.99;
        //magy *= 0.99;
    }
}

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

    

    // Gewichte für x und y, um Kurve zu glätten
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

void PerlinNoise2D::createGrid()
{
    std::cout << "Create grid..." << "\n";
	std::ofstream myfile;
	myfile.open("values.txt");
	
	for (int i = 0; i < YPOS; i++)
	{
        for (int j = 0; j < XPOS; j++)
        {
            double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
            myfile << vector[0] << " " << vector[1] << "\n";
        }
	}
	myfile.close();
    std::cout << "Grid successfully created.\n";
}

double* PerlinNoise2D::getVector(unsigned int x0, unsigned int y0)
{
    //myfile.ignore
    double* vector = new double[2];
    std::ifstream myfile;
    myfile.open("values.txt");
    std::string myText;

    unsigned int i = 0;
    while (myfile >> myText)
    {
        if (i == (XPOS * y0 * 2)+x0*2)
        {
            //std::cout << myText << "\n";
            vector[0] = std::stod(myText.c_str());
            //std::cout << vector[0] << "\n";
        }   
        
        if (i == (XPOS * y0 * 2)+x0*2+1)
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

double* PerlinNoise2D_MEM::getVector(unsigned int x0,unsigned int y0)
{
    if (x0 >= XPOS) x0 = x0 % XPOS;
    if (y0 >= YPOS) y0 = y0 % YPOS;
    double* vector = new double[2]{ grid[x0][y0][0], grid[x0][y0][1] };
    return vector;
};

void PerlinNoise2D_MEM::createGrid()
{
    for (int i = 0; YPOS > i; i++)
    {
        for (int j = 0; XPOS > j; j++)
        {
            double* vector = calcUnitVector(0.0, 0.0, float_range(mt_rng));
            grid[j][i][0] = vector[0];
            grid[j][i][1] = vector[1];
            delete vector;
        }
    }
};