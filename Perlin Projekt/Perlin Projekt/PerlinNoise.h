#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <SFML/Graphics.hpp>

class PerlinNoise2D
{
public:
	PerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) : XPOS{ init_XPOS }, YPOS{ init_YPOS }, OFFSET_X{init_OFFSET_X}, OFFSET_Y{ init_OFFSET_Y }, SEED{ init_SEED }
	{
		mt_rng = std::mt19937(SEED);
		float_range = std::uniform_real_distribution<float>(0,360);
	};

	PerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_SEED) : PerlinNoise2D(init_XPOS, init_YPOS, 0, 0, init_SEED) {};
	PerlinNoise2D() : PerlinNoise2D(1,1,0,0,1) {};

	virtual void resetPerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) { XPOS = init_XPOS; YPOS = init_YPOS; OFFSET_X = init_OFFSET_X; OFFSET_Y = init_OFFSET_Y; SEED = init_SEED; };

	inline double lerp(double lo, double hi, double t);
	inline double dotProduct(double xi, double yi, double x, double y);
	double* calcUnitVector(double x, double y, double angle);
	inline double smoothening(double num);
	virtual double* getVector(unsigned int x0, unsigned int y0);
	double noise(double x, double y);
	void createGrid();
	void createPPMFile(std::string filename, double magx, double magy, double height, double amp, int grayscales);
	void drawNoise(unsigned int width, unsigned int height, double magx, double magy, double height_p, double amp, int grayscales);

protected:
	uint32_t XPOS, YPOS, SEED, OFFSET_X, OFFSET_Y;
	// Random Number Generator, initialisierung mit seed
	std::mt19937 mt_rng;
	std::uniform_real_distribution<float> float_range;
	
};

class PerlinNoise2D_MEM : public PerlinNoise2D
{
public:
	double* getVector(unsigned int x0, unsigned int y0) override;
	void createGrid();
	PerlinNoise2D_MEM(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) : PerlinNoise2D(init_XPOS, init_YPOS, init_OFFSET_X, init_OFFSET_Y, init_SEED)
	{
		allocateGrid();
		createGrid();
	};

	void allocateGrid()
	{
		grid = new double** [XPOS];
		for (int i = 0; i < XPOS; i++)
		{
			grid[i] = new double* [YPOS];
			for (int j = 0; j < YPOS; j++)
			{
				grid[i][j] = new double[2];
			}
		}
	};
	void deallocateGrid()
	{
		for (int i = 0; i < XPOS; i++)
		{	
			for (int j = 0; j < YPOS; j++)
			{
				delete grid[i][j];
			}
			delete grid[i];
		}
		delete grid;
	};

	PerlinNoise2D_MEM(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_SEED) : PerlinNoise2D_MEM(init_XPOS, init_YPOS, 0, 0, init_SEED) {};
	PerlinNoise2D_MEM() : PerlinNoise2D_MEM(1,1,0,0,1) {};
	void resetPerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) override;

private:
	double*** grid;
};
