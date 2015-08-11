#pragma once

#include <GL/glew.h>
#include <GL/SOIL.h>
#include <vector2.h>
#include <map>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Text
{
public:
	Text();
	//The font meta file// this will load the texture as well
	Text(char* file);
	~Text();

	GLuint texture;
	unsigned int charsWide;
	unsigned int charsTall;

	std::map<char, float> kernFactors;
	std::map<char, unsigned int> arrayPosition;

	vector2 getCoord(char character);
	float getKernFactor(char character);
};

