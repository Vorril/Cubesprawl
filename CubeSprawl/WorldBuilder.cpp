#include "WorldBuilder.h"


WorldBuilder::WorldBuilder()
{
}


WorldBuilder::~WorldBuilder()
{
}

void WorldBuilder::setCube(int x, int y, int z, CUBE type, bool evalVisiblityNow){
	Cube** voxel = worldFocus->getCubePosition(x, y, z);

	if (voxel == nullptr) return; // out of bounds
	if (*voxel != nullptr) return; // already filled // could do some overwrite semantics here, probably leave that responsability up to other methods

	switch (type)
	{
		case RED:
			 *voxel = new RedCube();
		break;
		case BLUE:
			 *voxel = new BlueCube();
		break;
		case EMPTY:
		
		default:
		//voxel = &chunkStacks[x][z]->stack[y];
		//if (*voxel == nullptr) *voxel = new Cube();
			*voxel = new BlueCube();
		break;
	}

		(*voxel)->setPosition(x, y, z);//a deferrend initialization step
		worldFocus->flagForBuffering(x, y, z);//TODO: choose whether or not something should be buffered// Note this flags a chunk

		if (evalVisiblityNow) {
			worldFocus->determineCubeExposedSides(*voxel);
			worldFocus->evaluateNeighborsHidden(x, y, z);

		}

}

void WorldBuilder::setCube_ChunkSpace(Chunk* focus, int x, int y, int z, CUBE type){
	if (focus == nullptr) return;

	Cube** voxel = focus->getCubePosition(x, y, z);

	switch (type)
	{
	case RED:
		*voxel = new RedCube();
		break;
	case BLUE:
		*voxel = new BlueCube();
		break;
	case EMPTY:

	default:
		//voxel = &chunkStacks[x][z]->stack[y];
		//if (*voxel == nullptr) *voxel = new Cube();
		*voxel = new BlueCube();
		break;
	}

	(*voxel)->setPosition(x+(16*focus->chunkX), y, z+(16*focus->chunkY));//a deferrend initialization step
	worldFocus->flagForBuffering(x, y, z);

}

void WorldBuilder::setChunksAsTerrain(int xCorner, int zCorner, int xWidth, int zDepth, float roughness){
	for (int x = xCorner; x < (xCorner + xWidth); x++){
		for (int z = zCorner; z < (zCorner + zDepth); z++){
			worldFocus->addChunk(x, z);
		}
	}
	int totWidth = 0;
	xWidth>zDepth ? totWidth = xWidth : totWidth = zDepth;
	totWidth *= 16;

	int size = 1;
	int pow = 0;

	while (size < totWidth){
		size *= 2;
		pow++;
		//0 -> 1
		//1 -> 2
		//2 -> 4
		//etc
	}

	TerrainGen minimumSquare = TerrainGen(pow, 1.0f);
	minimumSquare.generateChunk(0, 0);
	minimumSquare.scaleAll(15.0f);


	for (int zWorld = zCorner * 16; zWorld < ((zCorner + zDepth) * 16); zWorld++){
		for (int xWorld = xCorner * 16; xWorld < ((xCorner + xWidth) * 16); xWorld++){
			//maybe cieling here
			int height = (int)minimumSquare.getHeight(xWorld, zWorld);
			if (height<0)height = 0;
			Chunk** addingTo = worldFocus->getChunkPosition(xWorld, zWorld);
			if (height > (*addingTo)->heightFilled)
				(*addingTo)->heightFilled = height;

			for (int y = 0; y <= height; y++){
				if (y%2==0)(*addingTo)->chunkStacks[xWorld & 15][zWorld & 15]->stack[y] = new RedCube();
				else{ (*addingTo)->chunkStacks[xWorld & 15][zWorld & 15]->stack[y] = new BlueCube(); }
				(*addingTo)->chunkStacks[xWorld & 15][zWorld & 15]->stack[y]->setPosition(xWorld, y, zWorld);
			}//y
		}//x
	}//z

	for (int x = xCorner; x < (xCorner + xWidth); x++){
		for (int z = zCorner; z < (zCorner + zDepth); z++){
			worldFocus->determineChunkExposedSides(x, z);
		}
	}

	setBuffers();
}

void WorldBuilder::killCube(int x, int y, int z, bool evalNieghborsNow){
	Cube** voxel = worldFocus->getCubePosition(x, y, z);
	if (voxel == nullptr) return; // chunk DNE
	//////////////////////////////////////////////////////////

	if (*voxel != nullptr) { // definitely exists
		Chunk* cubeChunk = worldFocus->getChunkWorld(x, z);

		cubeChunk->exposedCubes.remove(*voxel);

		delete *voxel;
		*voxel = nullptr;//safe because this memory is in a container
		//Probably wouldnt be multithread safe

		
		if (evalNieghborsNow){
			worldFocus->evaluateNeighborsRevealed(x, y, z);
		}

		worldFocus->flagForBuffering(x, y, z);//probably redundant
	}
}

void WorldBuilder::setBuffers(){
	worldFocus->bufferAllFlagged();
}


