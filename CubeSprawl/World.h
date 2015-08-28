#pragma once

#include <Chunk.h>
#include <Shader.h>
#include <map>

class World
{
public:
	~World();
	void clear();
	static World* getInstance();
	Shader* shader; //TODO temp two of these is redundant
	Shader* normmapshader;
	GLuint texArray;
	GLuint texArray_NormMaps;

	void addChunk(int chuX, int chuY, int height = -1);

	//World coords
	Chunk* getChunkWorld(int x, int z)const;
	//Chunk coords
	Chunk* getChunk(int chunkX, int chunkY)const;
	//World coords (ints) return ptr 2 ptr
	Chunk** getChunkPosition(int x, int z);
	//World coords (floats)
	Chunk** getChunkPosition(float worldX, float worldZ);
	//If ** is nullptr the chunk DNE if * is nullptr the cube DNE
	Cube** getCubePosition(int x, int y, int z)const;
	//World coords
	Cube* getCube(int worldX, int y, int worldZ)const;



	//determine all exposed in one chunk
	void determineChunkExposed(int chunkX, int chunkY);
	//Determine exposed cubes and establish which sides are exposed
	//Use in conjunction with chunk.bufferAllExposedSides
	void determineChunkExposedSides(int chunkX, int chunkY);
	//Evaluate all sides of a cube and add it to exposedSides in its chunk
	//Will never remove exposed-edness; meant to be called on single additions
	//Follow up with evalNeighbors if desired
	void determineCubeExposedSides(int x, int y, int z);
	void determineCubeExposedSides(Cube* center);
	//After creating a cube neighbors may become hidden, call this to maybe hide them if need be
	void evaluateNeighborsHidden(int x, int y, int z);
	//After killing a cube nieghbors may become visible
	void evaluateNeighborsRevealed(int x, int y, int z);
	//sets a cube exposed given world coords
	__declspec(deprecated) void setExposed(int x, int y, int z, bool checkFirst = false);
	//Sided set exposed
	void setExposedSide(int x, int y, int z, int side);
	//Chunk coords


	bool chunkExists(int X, int Y)const;

	//Warning doesnt do hieght checking
	Cube* occupied(int x, int y, int z)const;

	//world coords
	int getHeight(int x, int z);




	void flagForBuffering(int x, int y, int z);
	void bufferAllFlagged();

	//Clicktest partitioned via chunk then dist sorted Cube hits
	Cube* clickTest(vector3 camPos, vector3 lookDir)const;
	//Side hit via CUBE_SIDE enum in Cube.h
	Cube* clickTestSide(vector3 camPos, vector3 lookDir, int& sideHit)const;

	void draw()const;

	void drawNormMapped()const;
	//Set fbo before this
	void drawToFBO()const;

	vector<Chunk*> worldChunks;//TODO move this back to private //TODO make this a list so modification doesnt invalidate pointers!!!!! Or just iterate map when needbe
private:
//SINGLETON/////
	World();
	static bool instantiated;
	static World* singleton;
	

	//Data containers
	map<int, Chunk*> chunkMap;
	int chunksWide = 1; int lowestX = 0; int highestX = 0;
	int chunksTall = 1; int lowestY = 0; int highestY = 0;
	int linearPosition(int x, int y)const;

	//Misc


	
};

