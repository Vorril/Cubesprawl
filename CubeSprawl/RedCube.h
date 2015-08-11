#pragma once
#include "Cube.h"

class RedCube :
	public Cube
{
public:
	RedCube();
	~RedCube();

	int getType()const override;
};

