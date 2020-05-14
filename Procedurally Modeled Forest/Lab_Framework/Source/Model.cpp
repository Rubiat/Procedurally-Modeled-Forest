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
		this->grid.push_back(vector<float>());
	}

	this->amplitude = 20.0f;
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
			this->grid[i].push_back(image[i][j]);
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
	
	for (int i = 0; i < (resolution + 1); i++) {
		for (int j = 0; j < (resolution + 1); j++) {
			height = getNoise(i, j) * this->amplitude;
			this->grid[i].push_back(height);
		}
	}
}

float Ground::getNoise(int x, int z)
{
	srand(x * 49632 + z * 325176 + this->seed);
	// Generate float from -1.0f to 1.0f
	float height = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f - 1.0f;
	return height;
}
