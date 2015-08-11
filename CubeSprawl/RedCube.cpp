#include "RedCube.h"
#include "WorldBuilder.h"

RedCube::RedCube() :Cube()
{
	type = getType();
}


RedCube::~RedCube()
{
}

int RedCube::getType()const{
	return (int)RED;
}
