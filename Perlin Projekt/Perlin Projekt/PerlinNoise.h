#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <string>
#include <SFML/Graphics.hpp>
#include "PerlinModifiers.h"
#include <vector>

class PerlinNoise2D
{
public:

	// Constructors
	PerlinNoise2D(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SUBSECTIONS_X, uint32_t init_SUBSECTIONS_Y, uint32_t init_SEED) : 
		SIZE_X{ init_SIZE_X }, SIZE_Y{ init_SIZE_Y }, SUBSECTIONS_X{ init_SUBSECTIONS_X }, SUBSECTIONS_Y{ init_SUBSECTIONS_Y }, SEED{ init_SEED }
	{
		mt_rng = std::mt19937(SEED);
		float_range = std::uniform_real_distribution<float>(0,360);
		
		//std::cout << "PerlinNoise2D Object has been created." << "\n";
	};

	PerlinNoise2D() : PerlinNoise2D(1,1,0,0,1) {};
	

	// Virtual Functions
	//virtual void resetPerlinNoise2D(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SEED) { SIZE_X = init_SIZE_X; SIZE_Y = init_SIZE_Y; SEED = init_SEED; };
	virtual double* getVector(uint32_t x0, uint32_t y0) = 0;
	virtual void createGrid() = 0;

	// Perlin Noise related Functions
	inline double lerp(double lo, double hi, double t);
	inline double dotProduct(double xi, double yi, double x, double y);
	double* calcUnitVector(double x, double y, double angle);
	inline double smoothening(double num);
	double noise(double x, double y);

	// Output Functions
	void createFile(std::string filename, std::string filetype, PerlinModifiers mod);
	void drawNoise(unsigned int width, unsigned int height, PerlinModifiers mod);
	std::string toString();
	std::mt19937& getRNG() { return mt_rng; };
	void printRNG() { std::cout << float_range(mt_rng) << "\n"; };
	static void skipObjectCount() { objectCount++; };
	//void threadTask(int i);
	//void threadWorker();




protected:
	const uint32_t SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED;
	std::mt19937 mt_rng;
	std::uniform_real_distribution<float> float_range;
	//static uint16_t total_maps = 0;
	static int objectCount;
	int objectID;



};

// Grid stored and accessed in Memory
class PerlinNoise2D_MEM : public PerlinNoise2D
{
public:
	
	// Constructors
	PerlinNoise2D_MEM(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SUBSECTIONS_X, uint32_t init_SUBSECTIONS_Y, uint32_t init_SEED) 
		: PerlinNoise2D(init_SIZE_X, init_SIZE_Y, init_SUBSECTIONS_X, init_SUBSECTIONS_Y, init_SEED)
	{
		allocateGrid();
		objectCount++;
		objectID = objectCount;
		//createGrid();

		std::cout << "PerlinNoise2D_MEM ("<<objectID<<") Object has been created." << "\n";

	};
	PerlinNoise2D_MEM() : PerlinNoise2D_MEM(1, 1, 0, 0, 1) {};

	// Exclusive Functions from derived Class
	void allocateGrid()
	{
		grid = new double** [SIZE_X];
		for (int i = 0; i < SIZE_X; i++)
		{
			grid[i] = new double* [SIZE_Y];
			for (int j = 0; j < SIZE_Y; j++)
			{
				grid[i][j] = new double[2];
			}
		}
	};
	void deallocateGrid()
	{
		for (int i = 0; i < SIZE_X; i++)
		{
			for (int j = 0; j < SIZE_Y; j++)
			{
				delete grid[i][j];
			}
			delete grid[i];
		}
		delete grid;
	};
	void createGrid() override;
	void createGridPart(uint32_t wholeMap_size_x, uint32_t wholeMap_size_y);
	void dumpGrid();
	std::string toString();
	

	// Overridden Functions
	/*void resetPerlinNoise2D(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SEED) override
	{
		deallocateGrid();
		PerlinNoise2D::resetPerlinNoise2D(init_SIZE_X, init_SIZE_Y, init_SEED);
		allocateGrid();
	};

	void resetPerlinNoise2D(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SEED, PerlinModifiers setMod) override 
	{ PerlinNoise2D::resetPerlinNoise2D(init_SIZE_X, init_SIZE_Y, init_SEED, setMod); };
	*/

	double* getVector(uint32_t x0, uint32_t y0) override;
	

private:
	double*** grid;

};

// Grid stored and accessed in Files
class PerlinNoise2D_FILE : public PerlinNoise2D
{
public:
	PerlinNoise2D_FILE(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_SUBSECTIONS_X, uint32_t init_SUBSECTIONS_Y, uint32_t init_SEED) 
		: PerlinNoise2D(init_SIZE_X, init_SIZE_Y, init_SUBSECTIONS_X, init_SUBSECTIONS_Y, init_SEED)
	{
		createGrid();
		std::cout << "PerlinNoise2D_FILE Object has been created." << "\n";
	};
	//void resetPerlinNoise2D(uint32_t init_SIZE_X, uint32_t init_SIZE_Y, uint32_t init_OFFSET_X, uint32_t init_OFFSET_Y, uint32_t init_SEED) override;
	double* getVector(uint32_t x0, uint32_t y0) override;
	void createGrid() override;
	
};