#pragma once

#include <Cube.h>
#include <vector>

using namespace std;

class CubeStack
{
public:
	CubeStack();
	CubeStack(int length);
	~CubeStack();

	
	int lengthAvail = 16;

	vector<Cube*> stack; 

	
};

