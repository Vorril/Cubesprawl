#include "World.h"
#include <algorithm>

bool World::instantiated = false;
World* World::singleton = nullptr;

static inline int int_floor(float x)
{
	int i = (int)x; /* truncate */
	return i - (i > x); /* convert trunc to floor */
}

World::World()
{
	instantiated = true;
}

World::~World()
{
	instantiated = false;
	for each (Chunk* var in worldChunks)
	{
		delete var;
	}
}

World* World::getInstance(){
	if (!World::instantiated) singleton = new World();
	return singleton;
}

void World::clear(){
	for each (Chunk* chunk in worldChunks){
		delete chunk;
	}

	worldChunks.clear();
	chunkMap.clear();
}

void World::addChunk(int chunkX, int chunkY, int height){
	if (chunkX > highestX){
		highestX = chunkX;
	}
	if (chunkX < lowestX){
		lowestX = chunkX;
	}
	if (chunkY > highestY){
		highestY = chunkY;
	}
	if (chunkY < lowestY){
		lowestY = chunkY;
	}
	chunksWide = highestX - lowestX + 1;
	chunksTall = highestY - lowestY + 1;

	if (chunkExists(chunkX, chunkY)) return;//coincidence proof

	Chunk* addMe = new Chunk(height);
	addMe->chunkX = chunkX;
	addMe->chunkY = chunkY;

	worldChunks.push_back(addMe);

	chunkMap.clear();
	for each (Chunk* remakeMap in worldChunks){
		chunkMap[linearPosition(remakeMap->chunkX, remakeMap->chunkY)] = remakeMap;
	}
}


Cube** World::getCubePosition(int x, int y, int z)const{
	int horz = int_floor(x / 16.0f);
	int vert = int_floor(z / 16.0f);

	if (!chunkExists(horz, vert)) return nullptr;

	int key = linearPosition(horz, vert);
	return chunkMap.at(key)->getCubePosition(x & 15, y, z & 15);//-1 = 15 when 
}

Cube* World::getCube(int worldX, int y, int worldZ)const{
	return getChunkWorld(worldX, worldZ)->getChunkCube(worldX & 15, y, worldZ & 15);
	
}

Chunk* World::getChunkWorld(int x,  int z)const{
	int horz = int_floor(x / 16.0f);
	int vert = int_floor(z / 16.0f);

	return getChunk(horz, vert);
}

Chunk* World::getChunk(int X, int Y)const{
	const int key = linearPosition(X, Y);
	if (!chunkMap.count(key) || chunkMap.at(key)->chunkX != X || chunkMap.at(key)->chunkY != Y) nullptr;
	return chunkMap.at(key);
}

Chunk** World::getChunkPosition(int x, int z){
	int horz = int_floor(x / 16.0f);
	int vert = int_floor(z / 16.0f);

	const int key = linearPosition(horz, vert);
	if (!chunkMap.count(key) || chunkMap.at(key)->chunkX != horz || chunkMap.at(key)->chunkY != vert) nullptr;
	return &(chunkMap.at(key));
}

Chunk** World::getChunkPosition(float x, float z){
	int horz = int_floor(x / 16.0f);
	int vert = int_floor(z / 16.0f);

	const int key = linearPosition(horz, vert);
	if (!chunkMap.count(key) || chunkMap.at(key)->chunkX != horz || chunkMap.at(key)->chunkY != vert) nullptr;
	return &(chunkMap.at(key));
}


//Division -> chunk coords
//Bitand -> position in the chunk space
Cube* World::occupied(int x, int y, int z)const{
	int horz = int_floor(x / 16.0f);
	int vert = int_floor(z / 16.0f);
	
	if (!chunkExists(horz,vert)) return nullptr;//technically niether the chunk DNE
	const int key = linearPosition(horz, vert);
	return chunkMap.at(key)->occupied(x & 15, y, z & 15);
}

bool World::chunkExists(int X, int Y)const{
	const int key = linearPosition(X, Y);
	if (!chunkMap.count(key) || chunkMap.at(key)->chunkX != X || chunkMap.at(key)->chunkY != Y) return false;
	return true;
}

//World coords in
int World::getHeight(int x, int z){
	Chunk** lookingAt = getChunkPosition(x, z);
	if (lookingAt == nullptr) return -1;
	return (*lookingAt)->height;
}



void World::setExposed(int x, int y, int z, bool checkFirst){
	Cube** focus = getCubePosition(x, y, z);
	if (focus == nullptr || *focus == nullptr || (*focus)->isExposed){//carefully remembering not to expose something already exposed...
		return;//
		//TODO
	}

	Chunk* owner = getChunkWorld(x, z);
	owner->exposedCubes.push_back(*focus);
	(*focus)->isExposed = true;
	owner->needsBuffered = true;
}

void World::setExposedSide(int x, int y, int z, int side){
	Cube** focus = getCubePosition(x, y, z);
	if (focus == nullptr || *focus == nullptr){
		return;
	}

	Chunk* owner = getChunkWorld(x, z);
	if ((*focus)->exposedSides == 0)//Not previously exposed;
		owner->exposedCubes.push_back(*focus);

	(*focus)->exposedSides |= side;

	owner->needsBuffered = true;
}

void World::determineCubeExposedSides(int x, int y, int z){
	Cube** center = getCubePosition(x, y, z);
	if (center == nullptr || *center == nullptr) return;

	int code = 0;
	if (y == (getHeight(x,z)-1) || !occupied(x, y + 1, z))code += 1;//above //  1
	if (y > 0 && !occupied(x, y - 1, z))code += 2;//below //  2
	if (!occupied(x, y, z - 1))code += 4;//left //  4
	if (!occupied(x, y, z + 1))code += 8; //right//  8
	if (!occupied(x - 1, y, z))code += 16;//back//   16 
	if (!occupied(x + 1, y, z))code += 32;  //foward//32

	if (code > 0){
		Chunk* owner = getChunkWorld(x, z);
		(owner->exposedCubes).push_back(*center);
		(*center)->exposedSides = code;
		owner->needsBuffered = true;
	}
}

void World::determineCubeExposedSides(Cube* center){
	int x = center->x;
	int y = center->y;
	int z = center->z;

	int code = 0;
	if (y == (getHeight(x, z) - 1) || !occupied(x, y + 1, z))code += 1;//above //  1
	if (y > 0 && !occupied(x, y - 1, z))code += 2;//below //  2
	if (!occupied(x, y, z - 1))code += 4;//left //  4
	if (!occupied(x, y, z + 1))code += 8; //right//  8
	if (!occupied(x - 1, y, z))code += 16;//back//   16 
	if (!occupied(x + 1, y, z))code += 32;  //foward//32

	if (code > 0){
		Chunk* owner = getChunkWorld(x, z);
		(owner->exposedCubes).push_back(center);
		(center)->exposedSides = code;
		owner->needsBuffered = true;
	}
}

//Not much error pre-emption expect bugs
void World::evaluateNeighborsHidden(int x, int y, int z){
	Cube** center = getCubePosition(x, y, z);
	if (center == nullptr || *center == nullptr) return;

	
	if (y > 0 ){
		if( Cube* below = occupied(x, y - 1, z)){
			below->exposedSides -= 1;// 1 = yUp // above is no longer exposed
			if (below->exposedSides == 0){
				getChunkWorld(x, z)->hide(below);
			} //below was found to be hidden
		}//below existed
	}//trying below

	if (y < getHeight(x, z) - 2){// highest = get - 1// that can never be hidden// so less than h - 2 might start to hide above
		if (Cube* above = occupied(x, y + 1, z)){
			above->exposedSides -= 2; // below is exposed = 2 // below is no longer exposed
			if (above->exposedSides == 0){
				getChunkWorld(x, z)->hide(above);
			}
		}
	}//above

	if (Cube* forw = occupied(x + 1, y, z)){//32 forward
		forw->exposedSides -= 16;
		if (forw->exposedSides == 0){
			getChunkWorld(x + 1, z)->hide(forw);
		}
	}//forw

	if (Cube* back = occupied(x - 1, y, z)){//16 back
		back->exposedSides -= 32;
		if (back->exposedSides == 0){
			getChunkWorld(x - 1, z)->hide(back);
		}
	}//back

	if (Cube* right = occupied(x, y, z + 1)){// 8 right
		right->exposedSides -= 4;
		if (right->exposedSides == 0){
			getChunkWorld(x, z + 1)->hide(right);
		}
	}//right

	if (Cube* left = occupied(x, y, z - 1)){// 4 left
		left->exposedSides -= 8;
		if (left->exposedSides == 0){
			getChunkWorld(x, z - 1)->hide(left);
		}
	}//left

}

void World::evaluateNeighborsRevealed(int x, int y, int z){
	if (y > 0){
		if (Cube* below = occupied(x, y - 1, z)){
			below->exposedSides += 1;// 1 = yUp // above is  exposed
			if (below->exposedSides == 1){//it wasnt already
				getChunkWorld(x, z)->reveal(below);
			} //below was found to be hidden
		}//below existed
	}//trying below

	if (y < getHeight(x, z) - 2){
		if (Cube* above = occupied(x, y + 1, z)){
			above->exposedSides += 2; // below is exposed = 2 // below is no longer exposed
			if (above->exposedSides == 2){
				getChunkWorld(x, z)->reveal(above);
			}
		}
	}//above

	if (Cube* forw = occupied(x + 1, y, z)){//32 forward
		forw->exposedSides += 16;
		if (forw->exposedSides == 16){
			getChunkWorld(x + 1, z)->reveal(forw);
		}
		else getChunkWorld(x + 1, z)->needsBuffered = true;
	}//forw

	if (Cube* back = occupied(x - 1, y, z)){//16 back
		back->exposedSides += 32;
		if (back->exposedSides == 32){
			getChunkWorld(x - 1, z)->reveal(back);
		}
		else getChunkWorld(x - 1, z)->needsBuffered = true;
	}//back

	if (Cube* right = occupied(x, y, z + 1)){// 8 right
		right->exposedSides += 4;
		if (right->exposedSides == 4){
			getChunkWorld(x, z + 1)->reveal(right);
		}
		else getChunkWorld(x, z + 1)->needsBuffered = true;
	}//right

	if (Cube* left = occupied(x, y, z - 1)){// 4 left
		left->exposedSides += 8;
		if (left->exposedSides == 8){
			getChunkWorld(x, z - 1)->reveal(left);
		}
		else getChunkWorld(x, z - 1)->needsBuffered = true;
	}//left
}


void World::determineChunkExposed(int chunkX, int chunkY){
	int key = linearPosition(chunkX, chunkY);
	if (!chunkMap.count(key) || chunkMap[key]->chunkX != chunkX || chunkMap[key]->chunkY != chunkY) return;//coincidence checking

	Chunk& focus = *chunkMap.at(key);
	focus.exposedCubes.clear();

	//using these coordinates and going through world->occupied lets us check neighboring chunks
	const int xCorner = focus.chunkX *16;
	const int zCorner = focus.chunkY *16;
	for (int x = xCorner; x < xCorner + 16; x++){
		for (int z = zCorner; z < zCorner + 16; z++){
			for (int y = 0; y < focus.height-1; y++){
				if (focus.occupied(x & 15, y, z & 15)){//kinda want to make look up functions mod if given out of bounds numbers... rather than remember to pass
					if (!occupied(x, y + 1, z)//above
						|| !occupied(x + 1, y, z) //left
							|| !occupied(x, y, z + 1) //right
							|| !occupied(x - 1, y, z) //foward
							|| !occupied(x, y, z - 1) //back
							){ 
						focus.exposedCubes.push_back(*focus.getCubePosition(x & 15, y, z & 15));
						focus.exposedCubes.back()->isExposed = true;
					}
				}//if it exists
			}//y // TODO: Consider looking from below mechanics...
			//Any cubes on top get a free pass // avoiding a vector out of bounds error above.
			if (focus.occupied(x & 15, focus.height - 1, z & 15))focus.exposedCubes.push_back(*focus.getCubePosition(x & 15, focus.height - 1, z & 15));
		}//z
	}//x

	

	focus.needsBuffered = true;
}//determineChunkExposed

void World::determineChunkExposedSides(int chunkX, int chunkY){
	int key = linearPosition(chunkX, chunkY);
	if (!chunkMap.count(key) || chunkMap[key]->chunkX != chunkX || chunkMap[key]->chunkY != chunkY) return;//coincidence checking

	Chunk& focus = *chunkMap.at(key);
	focus.exposedCubes.clear();

	//using these coordinates and going through world->occupied lets us check neighboring chunks
	const int xCorner = focus.chunkX * 16;
	const int zCorner = focus.chunkY * 16;
	for (int x = xCorner; x < xCorner + 16; x++){
		for (int z = zCorner; z < zCorner + 16; z++){
			//Do the bottom layer:
			if (Cube* center = focus.occupied(x & 15, 0, z & 15)){//cube exists
				int code = 0;
				if (!occupied(x, 1, z))code += 1;//above //  1
				if (!occupied(x, 0, z - 1))code += 4;//left //  4
				if (!occupied(x, 0, z + 1))code += 8; //right//  8
				if (!occupied(x - 1, 0, z))code += 16;//back//   16 
				if (!occupied(x + 1, 0, z))code += 32;  //foward//32
					

				center->exposedSides = code;
				focus.exposedCubes.push_back(center);

			}//checked the bot layer ignoring its bottom side
			
			for (int y = 1; y < focus.height - 1; y++){
				if (Cube* center = focus.occupied(x & 15, y, z & 15)){//cube exists
					int code = 0;
					if (!occupied(x, y+1, z))code += 1;//above //  1
					if (!occupied(x, y-1, z))code += 2;//below //  2
					if (!occupied(x, y, z - 1))code += 4;//left //  4
					if (!occupied(x, y, z + 1))code += 8; //right//  8
					if (!occupied(x - 1, y, z))code += 16;//back//   16 
					if (!occupied(x + 1, y, z))code += 32;  //foward//32
					
					center->exposedSides = code;
					focus.exposedCubes.push_back(center);

				}//Checked middle layer checking everything
			}//Middle layer
			
			//Top layer
			if (Cube* center = focus.occupied(x & 15, focus.height - 1, z & 15)){
				int code = 1;
				//if (!occupied(x, y + 1, z))code += 1;//above //  1//free pass
				if (!occupied(x, focus.height - 1 - 1, z))code += 2;//below //  2
				if (!occupied(x, focus.height - 1, z - 1))code += 4;//left //  4
				if (!occupied(x, focus.height - 1, z + 1))code += 8; //right//  8
				if (!occupied(x - 1, focus.height - 1, z))code += 16;//back//   16 
				if (!occupied(x + 1, focus.height - 1, z))code += 32;  //foward//32

				center->exposedSides = code;
				focus.exposedCubes.push_back(center);

			}//checked the top layer giving the top side a free pass

		}//z
	}//x



	focus.needsBuffered = true;
}

void World::flagForBuffering(int x, int y, int z){
	int horz = int_floor(x / 16.0f);
	int vert = int_floor(z / 16.0f);
	int key = linearPosition(horz, vert);
	if (!chunkMap.count(key)) return;

	if (chunkMap[key]->heightFilled < y)chunkMap[key]->heightFilled = y;//sneaky
	chunkMap[key]->needsBuffered = true;
}

void World::bufferAllFlagged(){
	for each (Chunk* var in worldChunks){
		if (var->needsBuffered){
			var->bufferAllVisibleSides();
		}
	}
}


int World::linearPosition(int x, int y)const{
	return (x*chunksTall) + y;
	
}


inline float dist(vector3 p0, vector3 n, vector3 l0, vector3 l){
	          //pointOnPlane/planeNormal//PointonLine/Linedir
	return ((p0 - l0)*n) / (l*n);
};
Cube* World::clickTest(vector3 camPos, vector3 lookDir)const{
	vector<Chunk*> chunksHit;//Go through chunks that might be clicked on then exposed cubes 
	
	float yNear, xNear, zNear;
	float yFar, xFar, zFar;
	const vector3 xNorm(1.0f, 0.0f, 0.0f);
	const vector3 zNorm(0.0f, 0.0f, 1.0f);
	const vector3 yNorm(0.0f, 1.0f, 0.0f);
	float smallestMax, greatestMin;

	////////////////////////////////
	/// Check Each Chunk for hit ///
	////////////////////////////////
	for each (Chunk* onScreenChunk in worldChunks){//TODO: only loop onscreen chunks
		smallestMax = 1000000.0f;
		greatestMin = -1000000.0f;

		///////////////////////////////////////////////////////////////////////////////////
		//// X Slab //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		vector3 xNearPlane(onScreenChunk->chunkX * 16.0f, 0.0f, 0.0f);
		xNear = dist(xNearPlane, xNorm, camPos, lookDir);
		xNearPlane[0] += 16.0f;//now xFarPlane
		xFar = dist(xNearPlane, xNorm, camPos, lookDir);
		if (xNear > xFar) swap(xNear, xFar);

		///////////////////////////////////////////////////////////////////////////////////
		//// Y Slab //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		vector3 yNearPlane(0.0f, 0.0f, 0.0f);
		yNear = dist(yNearPlane, yNorm, camPos, lookDir);
		yNearPlane[1] += (float)(onScreenChunk->heightFilled + 1);//now yFarPlane
		yFar = dist(yNearPlane, yNorm, camPos, lookDir);
		if (yNear > yFar) swap(yNear, yFar);

		///////////////////////////////////////////////////////////////////////////////////
		//// Z Slab //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		vector3 zNearPlane(0.0f, 0.0f, onScreenChunk->chunkY * 16.0f);
		zNear = dist(zNearPlane, zNorm, camPos, lookDir);
		zNearPlane[2] += 16.0f;//now zFarPlane
		zFar = dist(zNearPlane, zNorm, camPos, lookDir);
		if (zNear > zFar) swap(zNear, zFar);


		smallestMax = min(zFar, min(xFar, yFar));//didnt check agaisnt +infinity
		greatestMin = max(zNear, max(xNear, yNear));

		if (smallestMax > greatestMin)chunksHit.push_back(onScreenChunk);//finally found
		
	}//For chunks
	if (chunksHit.size()==0)return nullptr;
		

	///////////////////////////////////
	//////////// SUB HITS /////////////
	///////////////////////////////////
	vector<pair<Cube*, float>> subHits;

	for each (Chunk* hit in chunksHit){
		list<Cube*>::iterator iter;
		for (iter = hit->exposedCubes.begin(); iter != hit->exposedCubes.end(); iter++){
			smallestMax = 1000000.0f;
			greatestMin = -1000000.0f;

			///////////////////////////////////////////////////////////////////////////////////
			//// X Slab //////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			vector3 xNearPlane((float)((*iter)->x), 0.0f, 0.0f);
			xNear = dist(xNearPlane, xNorm, camPos, lookDir);
			xNearPlane[0] += 1.0f;//now xFarPlane
			xFar = dist(xNearPlane, xNorm, camPos, lookDir);
			if (xNear > xFar) swap(xNear, xFar);

			///////////////////////////////////////////////////////////////////////////////////
			//// Y Slab //////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			vector3 yNearPlane(0.0f, (float)((*iter)->y), 0.0f);
			yNear = dist(yNearPlane, yNorm, camPos, lookDir);
			yNearPlane[1] += 1.0f;//now yFarPlane
			yFar = dist(yNearPlane, yNorm, camPos, lookDir);
			if (yNear > yFar) swap(yNear, yFar);

			///////////////////////////////////////////////////////////////////////////////////
			//// Z Slab //////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			vector3 zNearPlane(0.0f, 0.0f, (float)((*iter)->z));
			zNear = dist(zNearPlane, zNorm, camPos, lookDir);
			zNearPlane[2] += 1.0f;//now zFarPlane
			zFar = dist(zNearPlane, zNorm, camPos, lookDir);
			if (zNear > zFar) swap(zNear, zFar);


			smallestMax = min(zFar, min(xFar, yFar));//didnt check agaisnt +infinity
			greatestMin = max(zNear, max(xNear, yNear));
			if (smallestMax > greatestMin && smallestMax <0.0f) subHits.push_back(make_pair(*iter, greatestMin));//finally found
					
			
		}// exposedCube iterator
	}//foreach chunk hit

	if (subHits.size() == 0)return nullptr;

	pair<Cube*,float> closestHitSub = subHits[0];

	for each (pair<Cube*,float> closeCubes in subHits) {
		if (closeCubes.second > closestHitSub.second) closestHitSub = closeCubes;
	}
	return closestHitSub.first;
}

Cube* World::clickTestSide(vector3 camPos, vector3 lookDir, int& sideHit)const{
	vector<Chunk*> chunksHit;//Go through chunks that might be clicked on then exposed cubes 

	float yNear, xNear, zNear;
	float yFar, xFar, zFar;
	const vector3 xNorm(1.0f, 0.0f, 0.0f);
	const vector3 zNorm(0.0f, 0.0f, 1.0f);
	const vector3 yNorm(0.0f, 1.0f, 0.0f);
	float smallestMax, greatestMin;

	////////////////////////////////
	/// Check Each Chunk for hit ///
	////////////////////////////////
	for each (Chunk* onScreenChunk in worldChunks){//TODO: only loop onscreen chunks
		smallestMax = 1000000.0f;
		greatestMin = -1000000.0f;

		///////////////////////////////////////////////////////////////////////////////////
		//// X Slab //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		vector3 xNearPlane(onScreenChunk->chunkX * 16.0f, 0.0f, 0.0f);
		xNear = dist(xNearPlane, xNorm, camPos, lookDir);
		xNearPlane[0] += 16.0f;//now xFarPlane
		xFar = dist(xNearPlane, xNorm, camPos, lookDir);
		if (xNear > xFar) swap(xNear, xFar);

		///////////////////////////////////////////////////////////////////////////////////
		//// Y Slab //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		vector3 yNearPlane(0.0f, 0.0f, 0.0f);
		yNear = dist(yNearPlane, yNorm, camPos, lookDir);
		yNearPlane[1] += (float)(onScreenChunk->heightFilled+1);//now yFarPlane
		yFar = dist(yNearPlane, yNorm, camPos, lookDir);
		if (yNear > yFar) swap(yNear, yFar);

		///////////////////////////////////////////////////////////////////////////////////
		//// Z Slab //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		vector3 zNearPlane(0.0f, 0.0f, onScreenChunk->chunkY * 16.0f);
		zNear = dist(zNearPlane, zNorm, camPos, lookDir);
		zNearPlane[2] += 16.0f;//now zFarPlane
		zFar = dist(zNearPlane, zNorm, camPos, lookDir);
		if (zNear > zFar) swap(zNear, zFar);


		smallestMax = min(zFar, min(xFar, yFar));//didnt check agaisnt +infinity
		greatestMin = max(zNear, max(xNear, yNear));

		if (smallestMax > greatestMin)chunksHit.push_back(onScreenChunk);//finally found

	}//For chunks
	if (chunksHit.size() == 0)return nullptr;


	///////////////////////////////////
	//////////// SUB HITS /////////////
	///////////////////////////////////
	vector<pair<Cube*, float>> subHits;

	for each (Chunk* hit in chunksHit){
		list<Cube*>::iterator iter;
		for (iter = hit->exposedCubes.begin(); iter != hit->exposedCubes.end(); iter++){
			smallestMax = 1000000.0f;
			greatestMin = -1000000.0f;

			///////////////////////////////////////////////////////////////////////////////////
			//// X Slab //////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			vector3 xNearPlane((float)((*iter)->x), 0.0f, 0.0f);
			xNear = dist(xNearPlane, xNorm, camPos, lookDir);
			xNearPlane[0] += 1.0f;//now xFarPlane
			xFar = dist(xNearPlane, xNorm, camPos, lookDir);
			if (xNear > xFar) swap(xNear, xFar);

			///////////////////////////////////////////////////////////////////////////////////
			//// Y Slab //////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			vector3 yNearPlane(0.0f, (float)((*iter)->y), 0.0f);
			yNear = dist(yNearPlane, yNorm, camPos, lookDir);
			yNearPlane[1] += 1.0f;//now yFarPlane
			yFar = dist(yNearPlane, yNorm, camPos, lookDir);
			if (yNear > yFar) swap(yNear, yFar);

			///////////////////////////////////////////////////////////////////////////////////
			//// Z Slab //////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			vector3 zNearPlane(0.0f, 0.0f, (float)((*iter)->z));
			zNear = dist(zNearPlane, zNorm, camPos, lookDir);
			zNearPlane[2] += 1.0f;//now zFarPlane
			zFar = dist(zNearPlane, zNorm, camPos, lookDir);
			if (zNear > zFar) swap(zNear, zFar);


			smallestMax = min(zFar, min(xFar, yFar));//didnt check agaisnt +infinity
			greatestMin = max(zNear, max(xNear, yNear));
			if (smallestMax > greatestMin && smallestMax <0.0f) subHits.push_back(make_pair(*iter, smallestMax));//finally found


		}// exposedCube iterator
	}//foreach chunk hit

	if (subHits.size() == 0)return nullptr;

	pair<Cube*, float> closestHitSub = subHits[0];

	for each (pair<Cube*, float> closeCubes in subHits) {
		if (closeCubes.second > closestHitSub.second) closestHitSub = closeCubes;
	}


	///////////////////////////////////////////////////////////////////////////////////
	//// X Slab //////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	vector3 xNearPlane((float)((closestHitSub.first)->x), 0.0f, 0.0f);
	xNear = dist(xNearPlane, xNorm, camPos, lookDir);
	xNearPlane[0] += 1.0f;//now xFarPlane
	xFar = dist(xNearPlane, xNorm, camPos, lookDir);
	//if (xNear > xFar) swap(xNear, xFar);

	///////////////////////////////////////////////////////////////////////////////////
	//// Y Slab //////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	vector3 yNearPlane(0.0f, (float)((closestHitSub.first)->y), 0.0f);
	yNear = dist(yNearPlane, yNorm, camPos, lookDir);
	yNearPlane[1] += 1.0f;//now yFarPlane
	yFar = dist(yNearPlane, yNorm, camPos, lookDir);
	//if (yNear > yFar) swap(yNear, yFar);

	///////////////////////////////////////////////////////////////////////////////////
	//// Z Slab //////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	vector3 zNearPlane(0.0f, 0.0f, (float)((closestHitSub.first)->z));
	zNear = dist(zNearPlane, zNorm, camPos, lookDir);
	zNearPlane[2] += 1.0f;//now zFarPlane
	zFar = dist(zNearPlane, zNorm, camPos, lookDir);
	//if (zNear > zFar) swap(zNear, zFar);

	if (closestHitSub.second == yNear)sideHit = CUBE_SIDE::BOT;
	else if (closestHitSub.second == yFar)sideHit = CUBE_SIDE::TOP;
	else if (closestHitSub.second == xNear)sideHit = CUBE_SIDE::LEFT;
	else if (closestHitSub.second == xFar)sideHit = CUBE_SIDE::RIGHT;
	else if (closestHitSub.second == zNear)sideHit = CUBE_SIDE::BACK;
	else if (closestHitSub.second == zFar)sideHit = CUBE_SIDE::FORW;
		

	return closestHitSub.first;
}


void World::draw()const{
	glUseProgram(shader->theProgram);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // uv tex coords
	glEnableVertexAttribArray(2); // array

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
	glBindSampler(0, shader->samplers[0]);
	

	for each (Chunk* chunkPtr in worldChunks){

		chunkPtr->draw();

	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glUseProgram(0);
}

void World::drawNormMapped()const{
	glUseProgram(normmapshader->theProgram);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // uv tex coords
	glEnableVertexAttribArray(2); // array
	glEnableVertexAttribArray(3); // tangent
	glEnableVertexAttribArray(4); // bittangent
	glEnableVertexAttribArray(5); // norm


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
	glBindSampler(0, normmapshader->samplers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray_NormMaps);
	glBindSampler(1, normmapshader->samplers[1]);


	for each (Chunk* chunkPtr in worldChunks){

		chunkPtr->drawNormMapped();

	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);

	glUseProgram(0);
}

void World::drawToFBO()const{
	glUseProgram(shader->theProgram);

	glEnableVertexAttribArray(0); // output to renderBuffer (?)
	glEnableVertexAttribArray(1); // vert/ID


	for each (Chunk* chunkPtr in worldChunks){

		chunkPtr->drawToFBO();

	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glUseProgram(0);
}


