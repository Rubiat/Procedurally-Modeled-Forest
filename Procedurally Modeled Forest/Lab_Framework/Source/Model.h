#pragma once
#include "COMP371Helper.h"
#include <vector>

class Model
{
public:

	BoundingBox box;
	vec3 translationVector;
	vec3 scaleVector;
};

class Cube : public Model
{
public:

	Cube();
	Cube(vec3 translation, vec3 scaleVector);

};

class Tree : public Model
{
public:
	// Class tree will need to be modified based on the .obj file that we choose
	Tree();
	Tree(vec3 translation, vec3 scaleVector);

};

class Wolf : public Model
{
public:
	// Class tree will need to be modified based on the .obj file that we choose
	Wolf();
	Wolf(vec3 translation, vec3 scaleVector);

};

class Ground : public Model
{
public:
	// Class tree will need to be modified based on the .obj file that we choose
	Ground();
	~Ground();

	// makeHeightMap() is not used
	GLuint makeHeightMap(int seed, int zoom, double persistence, float minHeight, float maxHeight);

	void generateHeight(int resolution); // Ex: 128 for 128x128, 256 for 256x256
	float getHeight(int x, int z);
	vec3 calculateNormal(int x, int z);

	float getNoise(int x, int z);
	float getSmoothNoise(int x, int z);

	float interpolate(float a, float b, float blend);
	float getInterpolatedNoise(float x, float z);

	vector<vector<vector<float>>> grid;
	float amplitude;
	int octaves;
	int roughness;
	int seed;

};