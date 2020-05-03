#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <SFML/Graphics.hpp>
#include "PerlinModifiers.h"

class PerlinNoise2D
{
public:

	// Constructors
	PerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) : XPOS{ init_XPOS }, YPOS{ init_YPOS }, OFFSET_X{init_OFFSET_X}, OFFSET_Y{ init_OFFSET_Y }, SEED{ init_SEED }
	{
		mt_rng = std::mt19937(SEED);
		float_range = std::uniform_real_distribution<float>(0,360);
	};
	PerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_SEED) : PerlinNoise2D(init_XPOS, init_YPOS, 0, 0, init_SEED) {};
	PerlinNoise2D() : PerlinNoise2D(1,1,0,0,1) {};

	// Virtual Functions
	virtual void resetPerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) { XPOS = init_XPOS; YPOS = init_YPOS; OFFSET_X = init_OFFSET_X; OFFSET_Y = init_OFFSET_Y; SEED = init_SEED; };
	virtual double* getVector(uint32_t x0, uint32_t y0) = 0;
	virtual void createGrid() = 0;

	// Perlin Noise related Functions
	inline double lerp(double lo, double hi, double t);
	inline double dotProduct(double xi, double yi, double x, double y);
	double* calcUnitVector(double x, double y, double angle);
	inline double smoothening(double num);
	double noise(double x, double y);

	// Output Functions
	void createPPMFile(std::string filename, double magx, double magy, double height, double amp, int grayscales);
	void drawNoise(unsigned int width, unsigned int height, double magx, double magy, double height_p, double amp, int grayscales);



protected:
	uint32_t XPOS, YPOS, SEED, OFFSET_X, OFFSET_Y;
	std::mt19937 mt_rng;
	std::uniform_real_distribution<float> float_range;
	PerlinModifiers mod;
};

// Grid stored and accessed in Memory
class PerlinNoise2D_MEM : public PerlinNoise2D
{
public:
	
	// Constructors
	PerlinNoise2D_MEM(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) : PerlinNoise2D(init_XPOS, init_YPOS, init_OFFSET_X, init_OFFSET_Y, init_SEED)
	{
		allocateGrid();
		createGrid();
	};
	PerlinNoise2D_MEM(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_SEED) : PerlinNoise2D_MEM(init_XPOS, init_YPOS, 0, 0, init_SEED) {};
	PerlinNoise2D_MEM() : PerlinNoise2D_MEM(1, 1, 0, 0, 1) {};

	// Exclusive Functions from derived Class
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
	void createGrid();

	// Overridden Functions
	void resetPerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) override
	{
		deallocateGrid();
		PerlinNoise2D::resetPerlinNoise2D(init_XPOS, init_YPOS, init_OFFSET_X, init_OFFSET_Y, init_SEED);
		allocateGrid();
	};
	double* getVector(uint32_t x0, uint32_t y0) override;
	

private:
	double*** grid;
};

// Grid stored and accessed in Files
class PerlinNoise2D_FILE : public PerlinNoise2D
{
	//void resetPerlinNoise2D(uint32_t init_XPOS, uint32_t init_YPOS, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) override;
	double* getVector(uint32_t x0, uint32_t y0) override;
	void createGrid();
	
};