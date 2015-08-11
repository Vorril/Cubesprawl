#include "Text.h"


Text::Text()
{
}
Text::Text(char* file){
	

	std::ifstream source;
	source.open(file);

	std::string line1;

	std::getline(source, line1);
	//std::istringstream in0(line1);
	std::string textFile(line1);
	//in0 >> textFile;

	std::string textLocation(file);
	int slashIndex = textLocation.find_last_of('/');
	std::string textLoc(textLocation.substr(0, slashIndex+1));//+1 may cause an error if for some reason you load from a root directory

	texture = SOIL_load_OGL_texture((textLoc+textFile).c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_COMPRESS_TO_DXT // SOIL_FLAG_NTSC_SAFE_RGB |
		);




	std::getline(source, line1);
	std::istringstream in1(line1);
	in1 >> charsWide;

	std::getline(source, line1);
	std::istringstream in2(line1);
	in2 >> charsTall;

	for (std::string line; std::getline(source, line);){
		std::istringstream in(line);
		char key;
		unsigned int pos;
		float kern;
		in >> key;
		in >> pos;
		in >> kern;

		kernFactors[key] = kern;
		arrayPosition[key] = pos;

	}

	kernFactors[' '] = 0.55f;
	arrayPosition[' '] = 28;

	source.close();

}

Text::~Text()
{
	//gldeletetexture
}

vector2 Text::getCoord(char character){
	float xUnit = 1.0f / charsWide;
	float yUnit = 1.0f / charsTall;

	if (arrayPosition.count(character)){
		unsigned int key = arrayPosition[character];

	unsigned int x = key % charsWide;
	unsigned int y = key / charsWide;

	return vector2(x * xUnit, y * yUnit);
}

	return vector2(0.9f, 0.9f);


}

float Text::getKernFactor(char character){
	if (kernFactors.count(character)){
		return kernFactors[character];
	}
	else
		return 1.0f;
}