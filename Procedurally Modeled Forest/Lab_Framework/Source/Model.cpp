#include "Model.h"

Cube::Cube()
{
	translationVector = vec3(0.0f, 0.0f, 0.0f);
	scaleVector = vec3(1.0f, 1.0f, 1.0f);
	vec3 pos(0.0f, 0.0f, 0.0f);
	box = BoundingBox(pos, 5.0f, 5.0f, 5.0f);
	// The 5.0f size value is hardcoded because that was the value 
	// of the size based on Cube.obj (the pos also)
}

Cube::Cube(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	vec3 pos(0.0f, 0.0f, 0.0f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		5.0f * scale.x, 5.0f * scale.y, 5.0f * scale.z);
}

Tree::Tree()
{
	translationVector = vec3(0.0f, 0.0f, 0.0f);
	scaleVector = vec3(1.0f, 1.0f, 1.0f);
	//vec3 pos(-0.1483415f, 4.9745075f, -0.0472805f);
	vec3 pos(-0.1483415f, 0.0f, -0.0472805f);
	box = BoundingBox(pos, 0.8139465f, 10.140205f, 0.7113755f);
}

Tree::Tree(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	//vec3 pos(-0.1483415f, 4.9745075f, -0.0472805f);
	vec3 pos(-0.1483415f, 0.0f, -0.0472805f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		0.8139465f * scale.x, 10.140205f * scale.y, 0.7113755f * scale.z);
}

Wolf::Wolf()
{
	translationVector = vec3(0.0f, 3.5f, 0.0f);
	scaleVector = vec3(1.0f, 1.0f, 1.0f);
	vec3 pos(0.0f, -0.5744805f, 0.0039735f);
	box = BoundingBox(pos, 1.082057f, 2.9391915f, 4.0801215f);
}

Wolf::Wolf(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	vec3 pos(0.0f, -0.5744805f, 0.0039735f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		1.082057f * scale.x, 2.9391915f * scale.y, 4.0801215f * scale.z);
}

Ground::Ground()
{
	for (int i = 0; i < 129; i++) {
		this->grid.push_back(vector<vector<float>>());
		for (int j = 0; j < 129; j++) {
			this->grid[i].push_back(vector<float>());
		}
	}

	this->amplitude = 10.0f;
	this->octaves = 3;
	this->roughness = 3.0f; // 3.0f for 1/3 == 1/roughness
	this->seed = (rand() % 1000000000);
}

Ground::~Ground()
{
	/*for (int i = 0; i < 128; i++) {
		delete[] grid[i];
	}
	delete[] grid;*/
}

GLuint Ground::makeHeightMap(int seed, int zoom, double persistence, float minHeight, float maxHeight) {
	PerlinNoise myNoise(seed, zoom, persistence);
	GLfloat image[129][129];
	//Have to keep 'image' so make a member vector<vector<float>> instead for the Ground class
	// and insert the values into that as well, instead of using the same array to store and load
	// the image as well.

	double c;

	for (int i = 0; i < 129; i++)
	{
		for (int j = 0; j < 129; j++)
		{
			c = (1.0 + myNoise.perlinNoise2D(8, i, j)) / 2.0;
			image[i][j] = (float)interpolate(minHeight, maxHeight, c);
			this->grid[i][j].push_back(image[i][j]);
			//cout << ((image[i][j]*1000)/10.0f) << " ";
		}
		//cout << endl;
	}

	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 129, 129, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);


	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

void Ground::generateHeight(int resolution)
{
	float height;
	vec3 normal;
	
	for (int i = 0; i < (resolution + 1); i++) {
		for (int j = 0; j < (resolution + 1); j++) {
			height = getHeight(i, j);
			// We do it three (octaves) times to add more small frequencies 

			normal = calculateNormal(i, j);

			this->grid[i][j].push_back(height);
			this->grid[i][j].push_back(normal.x);
			this->grid[i][j].push_back(normal.y);
			this->grid[i][j].push_back(normal.z);
		}
	}
}

float Ground::getHeight(int x, int z)
{
	float height = 0;
	float multiplier = 1.0f;
	float roughness = 1.0f;

	for (int i = 0; i < this->octaves; i++) {
		height += getInterpolatedNoise(x / 12.0f * multiplier, z / 12.0f * multiplier)
			* this->amplitude * roughness;
		multiplier *= 2.0f; // Multiply by 2
		roughness *= (1.0f / this->roughness);
	}

	return height;
}

vec3 Ground::calculateNormal(int x, int z)
{
	float heightLeft = getHeight(x-1, z);
	float heightRight = getHeight(x + 1, z);
	float heightDown = getHeight(x, z - 1);
	float heightUp = getHeight(x, z + 1);

	vec3 normal = vec3(heightLeft - heightRight, 2.0f, heightDown - heightUp);
	normal = normalize(normal);

	return normal;
}

//--------------------------------------------------------------------------------------------//

float Ground::getNoise(int x, int z)
{
	srand(x * 49632 + z * 325176 + this->seed);
	// Generate float from -1.0f to 1.0f
	float height = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f - 1.0f;
	return height;
}

float Ground::getSmoothNoise(int x, int z)
{
	float corners = (getNoise(x-1, z-1) + getNoise(x+1, z-1) + getNoise(x-1, z+1) + getNoise(x+1, z+1))
		/ 16.0f; // Divide by 16 to minimize the effect of the corner points
	float sides = (getNoise(x-1, z) + getNoise(x+1, z) + getNoise(x, z-1) + getNoise(x, z+1))
		/ 8.0f; // Divide by 8 this time
	float center = (getNoise(x, z)) / 4.0f; // The main point, divide it by 4 only

	return corners + sides + center;
}

float Ground::interpolate(float a, float b, float blend)
{
	double theta = blend * 3.14159265359;
	float f = (float)(1.0f - cosf(theta)) * 0.5f;
	return a * (1.0f - f) + b * f;
}

float Ground::getInterpolatedNoise(float x, float z)
{
	int intX = (int)x;
	int intZ = (int)z;

	float fracX = x - intX;
	float fracZ = z - intZ;

	float v1 = getSmoothNoise(intX, intZ);
	float v2 = getSmoothNoise(intX + 1, intZ);
	float v3 = getSmoothNoise(intX, intZ + 1);
	float v4 = getSmoothNoise(intX + 1, intZ + 1);

	float i1 = interpolate(v1, v2, fracX);
	float i2 = interpolate(v3, v4, fracX);

	return interpolate(i1, i2, fracZ);
}
