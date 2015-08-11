#pragma once
#include "Cube.h"


class BlueCube :
	public Cube
{
public:
	BlueCube();
	~BlueCube();

	int getType()const override;
};

