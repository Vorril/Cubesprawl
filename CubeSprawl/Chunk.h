#pragma once

#include <CubeStack.h>
#include <vec4VBO.h>
#include <list>

class Chunk{

public:
	Chunk();
	//-1 is default un-initialized
	Chunk(int height);
	~Chunk();

	//TODO 
	CubeStack* chunkStacks[16][16];

	int chunkX;
	int chunkY;

	int height;
	int heightFilled = 0;

	vec4VBO buffer;

	bool needsBuffered;

	//not sure about consts roll with it for now
	//World will fill this because neighboring chunks must consider eachother
	list<Cube* > exposedCubes;







	//Buffer everything// legacy
	__declspec(deprecated) void bufferAll();
	//Buffer all cubes which were determined to have at least one exposed side
	__declspec(deprecated) void bufferAllVisible();
	//(Re-)Buffer only exposed faces determined by world.determineExposedSides(on chunkGen) and kept up to date with sided methods thereafter
	void bufferAllVisibleSides();


	int getChunkType(int x, int y, int z)const;
	Cube* getChunkCube(int x, int y, int z);
	Cube** getCubePosition(int x, int y, int z)const;
	//Warning doesnt check for hieght
	Cube* occupied(int x, int y, int z)const;

	//Remove from the list of exposed cubes (and buffer flag)
	void hide(int x, int y, int z);
	void hide(Cube* noLongerVisable);
	//Add to the list of exposed cubes (and buffer flag)
	void reveal(int x, int y, int z);
	void reveal(Cube* nowVisable);


	//Bind the program first this binds buffers and calls pointers and draws
	//Doesnt bind texture
	void draw()const;
	void drawNormMapped()const;
	//Bind the program first this binds buffers and calls pointers and draws
	void draw(Shader* texShader, GLuint tex)const;
	void drawToFBO()const;
};


