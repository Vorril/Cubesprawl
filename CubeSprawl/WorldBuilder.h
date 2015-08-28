#pragma once

#include "World.h"
#include "TerrainGen.h"
#include "BlueCube.h"
#include "RedCube.h"
#include "HomeCube.h"

class WorldBuilder
{
public:
	WorldBuilder();
	~WorldBuilder();

	World* worldFocus;

	//Set a spot in thw world to ocube of desired type provided the corresponding chunk exists
	//DetermineVis: Set true for granular adjustments and update immediately
	//Leave false for batch adjustments then follow with whole chunk recalculations as desired (ie world.determineChunkExposed)
	//Either way buffer update responsabilities are not here, merely flags are set
	void setCube(int x, int y, int z, CUBE type, bool determineVisibilityNow = false);
	void setCube_ChunkSpace(Chunk* focus, int x, int y, int z, CUBE type);
	void setChunksAsTerrain(int xCorner, int zCorner, int xWidth, int zDepth, int high = -1, float roughness = 1.0f);
	void killCube(int x, int y, int z, bool evalNieghborsNow = false);
	//Update anything flagged call this simply once per frame? Consider distributed processing
	void updateBuffers();
	
	
};

