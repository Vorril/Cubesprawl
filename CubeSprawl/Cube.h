#pragma once
#include <utility>

typedef unsigned int uint;
typedef std::pair<int, int> coordi;

enum CUBE{HOME, RED, BLUE, EMPTY};
enum CUBE_SIDE{TOP, BOT, LEFT, RIGHT, FORW, BACK};

class Cube
{
public:
	Cube();
	~Cube();

	//Worldspace positions
	int x, y, z;
	//Probably outdated and inaccurate use exposed sides
	bool isExposed = false;

	int exposedSides=0;
	//return a code unique to
	virtual int getType()const=0;

	int type;

	void setPosition(int setX, int setY, int setZ);



};

