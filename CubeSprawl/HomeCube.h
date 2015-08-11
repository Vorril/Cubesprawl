#pragma once
#include "Cube.h"


class HomeCube:
		public Cube
{
public:
	HomeCube();
	~HomeCube();


	int getType()const override;
};

