#pragma once
#include <math.h>

#include <vec4VBO.h>
#include <Shader.h>
#include <map>


#include <customMath.h>

#define DEBUG_ADDITIONAL

/*
Randomly generated terrain using fractal algorithms

*/
class TerrainGen
{
public:
	TerrainGen();
	TerrainGen(int LoD);
	TerrainGen(int LoD, float rough);
	~TerrainGen();

	void clean();


	vec4VBO surfaceMesh;

	/////////////////////////
	// CHUNK PROPERTIES  ////
	/////////////////////////////////////////////////////////////////////////////////////////////
	std::pair<int, int> chunkCoords;
	float width = 700.0f;
	int size; //257=2^8 + 1 // The width and hieght of our square chunks
	float roughness = 1.0f;

	int individualIndexSize;
	GLuint index = 0;
	float* hmap;
	/////////////////////////////////////////////////////////////////////////////////////////////


	void generateTestChunk(int xChunk, int yChunk, int type);
	void generateChunk(int xChunk, int yChunk);
	void generateChunkCornered(int xChunk, int yChunk, float TL, float TR, float BL, float BR);
	int indexSize;
	void makeIndices();

	//Scale all to be at most this high
	void scaleAll(float maxSet);
	float getHeight(int x, int y);

	void draw();
	void draw(Shader* terrainShader, GLuint texture);
	//draw within a call from map.draw() ie with indices etc already bound
	void drawFromMap();

	vec4VBO debugNormLines;
	void drawLines(Shader* linesShader);


	void calcNormalColumn(int x);
	void calcNormalRow(int y);
	void smoothVertsColumn(int x, bool horzPass = false);
	void smoothVertsRow(int y, bool vertPass = false);
private:
	void divide(int sizeRec);
	void square(int x, int y, int size, float offset);
	void diamond(int x, int y, int size, float offset);

	//deprecated
	//void genNormals();
	void genIndivNormals();

};
