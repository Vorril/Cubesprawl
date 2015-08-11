#include "TerrainGen.h"
#include <cstdlib>
#include <time.h>


#define randDec (rand()/(float)RAND_MAX)
#define random(min, max) ((randDec* (max - min + 1)) + min)
#define coordVec(x, y) surfaceMesh.verticies[(y)*size + (x)].coords[1]
#define coord(x, y) hmap[(y)*size + (x)]




TerrainGen::TerrainGen()
{
}
TerrainGen::TerrainGen(int LoD)
{
	size = std::pow(2, LoD) + 1;
}
TerrainGen::TerrainGen(int LoD, float rough){
	size = std::pow(2, LoD) + 1;
	roughness = rough;
}
TerrainGen::~TerrainGen()
{
	surfaceMesh.cleanup();
	if (hmap != nullptr)
		delete hmap;
}



void TerrainGen::generateChunk(int xChunk, int yChunk){
	if (hmap == nullptr)
	hmap = new float[size*size]();//size*size

	srand(time(NULL));

	//if (coord(0,0) != 0.0f)
		coord(0, 0) = (float)random(5, 11);
	//if (coord(0, size - 1) != 0.0f)
		coord(0, size - 1) = (float)random(5, 11);
	//if (coord(size - 1, 0) != 0.0f)
		coord(size - 1, 0) = (float)random(5, 11);
	//if (coord(size - 1, size - 1) != 0.0f)
	coord(size - 1, size - 1) = (float)random(5, 11);

	
	//MEAT//////////
	divide(size - 1);
	//MEAT//////////


	vector<float> guassKern;
	myMath::getGuassKern(&guassKern);

	myMath::convolveSqSepKern(hmap, size, guassKern, guassKern);

	//gen VBO
	float sizef = (float)size;

	//NOT NEEDED IN CUBEGAME
	//for (int h = 0; h < size; h++){  //h = y = height
	//	for (int w = 0; w < size; w++){ //w = x = width          
	//		surfaceMesh.add(width * (w / (sizef - 1.0f)) + width*xChunk, coord(w, h), width * (h / (sizef - 1.0f)) + width*yChunk);

	//	}
	//}

	//surfaceMesh.genVertBuffer(GL_STATIC_DRAW);//NOT NEEDED IN CUBEGAME

	///////////////////
	// Gen an index for this using the closest-diagonals appraoch

	//makeIndices();//NOT NEEDED IN CUBEGAME

	//genIndivNormals();//NOT NEEDED IN CUBEGAME

	//hmap should be done with now? // not until later in addChunk
	
}

void TerrainGen::scaleAll(float maxSet){
	float highestVal = -10000.0f;
	for (int x = 0; x < size; x++){
		for (int y = 0; y < size; y++){
			if (coord(x, y) > highestVal) highestVal = coord(x, y);
		}
	}
	float scale = maxSet / highestVal;
	for (int x = 0; x < size; x++){
		for (int y = 0; y < size; y++){
			coord(x, y) *= scale;
		}
	}
}

float TerrainGen::getHeight(int x, int y){
	return coord(x, y);
}

void TerrainGen::generateChunkCornered(int xChunk, int yChunk, float TL, float TR, float BL, float BR){
	if (hmap == nullptr)
		hmap = new float[size*size]();//size*size

	srand(time(NULL));

		coord(0, 0) = TL;
		coord(0, size - 1) = BL;
		coord(size - 1, 0) = TR;
		coord(size - 1, size - 1) = BR;



	divide(size - 1);

	vector<float> guassKern;
	myMath::getGuassKern(&guassKern);

	myMath::convolveSqSepKern(hmap, size, guassKern, guassKern);

	//gen VBO
	float sizef = (float)size;


	for (int h = 0; h < size; h++){  //h = y = height
		for (int w = 0; w < size; w++){ //w = x = width          
			surfaceMesh.add(width * (w / (sizef - 1.0f)) + width*xChunk, coord(w, h), width * (h / (sizef - 1.0f)) + width*yChunk);

		}
	}
	surfaceMesh.genVertBuffer(GL_STATIC_DRAW);

	///////////////////
	// Gen an index for this using the closest-diagonals appraoch

	makeIndices();

	genIndivNormals();

	//hmap should be done with now? // not until later in addChunk

}

void TerrainGen::generateTestChunk(int xChunk, int yChunk, int type){
	if (hmap != nullptr) delete[] hmap;
	hmap = new float[size*size];//size*size
	//chunkCoords = std::make_pair(xChunk, yChunk);


	switch (type){

	case 1:{
		float midP = size / 2.0f;
		float distSq = 1.0f;
		float height = size*0.8f;
			for (int x = 0; x < size; x++){
				for (int y = 0; y < size; y++){
					distSq = (midP - x)*(midP - x) + (midP - y)*(midP - y);
					coord(x, y) = height - 0.01f*(distSq) ;
				}
			}
	}
	case 2:{
		  for (int x = 0; x < size; x++){
		   for (int y = 0; y < size; y++){
			   
			   coord(x, y) = 40*sinf(8 * 3.141f*x / size) * sinf(8 * 3.141f*y / size);
		   }
	   }

	}


	default:
		break;
	}
	




	float sizef = (float)size;

	for (int w = 0; w < size; w++){ //w = x = width               
		for (int h = 0; h < size; h++){  //h = y = height
			surfaceMesh.add(width * (w / (sizef - 1.0f)) + width*xChunk, coord(w, h), width * (h / (sizef - 1.0f)) + width*yChunk);

		}
	}
	surfaceMesh.genVertBuffer(GL_STATIC_DRAW);

	genIndivNormals();
}


void TerrainGen::divide(int sizeRec){
	//cout << randDec << endl;
	int x, y;
	float half = sizeRec / 2.0f;
	float scale = roughness * sizeRec;

	while (half >= 1.0f){
	//if (half < 1.0f) return;

	for (y = half; y < (size-1); y += sizeRec) {
		for (x = half; x < (size-1); x += sizeRec) {
			if (coord(x,y) == 0.0f)
				square(x, y, half, randDec * scale * 2 - scale);		
		
		}
	}
	for (y = 0; y <= (size - 1); y += (int)half) {
		for (x = ((y + (int)half) % sizeRec); x <= (size - 1); x += sizeRec) {
			if (coord(x, y) == 0.0f) // ignore pre-seeded
				diamond(x, y, half, randDec * scale * 2 - scale);
		/*	else{
				if(y != 0)
				coord(x, y) += 0.0f;
			}*/
		}
	}
	//divide(sizeRec / 2);
	sizeRec /= 2;
	half = sizeRec / 2.0f;
	scale = roughness * sizeRec;
	}
}

void TerrainGen::square(int x, int y, int half, float offset){
	int c = 0;
	float avg = 0.0f;

	/*
	if (y + half < size && x + half < size){ avg += coord(x + half, y + half); c++; } //TR
	if (x + half < size && y - half > 0)   { avg += coord(x + half, y - half); c++; } //BR
	if (y + half < size && x - half > 0  ) { avg += coord(x - half, y + half); c++; } //TL
	if (x - half > 0 && y - half > 0)      { avg += coord(x - half, y - half); c++; } //BL
*/

	{ avg += coord(x + half, y + half); c++; } //TR
	  { avg += coord(x + half, y - half); c++; } //BR
	 { avg += coord(x - half, y + half); c++; } //TL
	   { avg += coord(x - half, y - half); c++; } //BL


	if (c != 0){
		avg /= c;
		coord(x, y) = avg;
		//coord(x, y) += offset;
	}
	coord(x, y) += offset;

}

void TerrainGen::diamond(int x, int y, int half, float offset){
	int c = 0;
	float avg = 0.0f;

	if (y + half < size){ avg += coord(x, y + half); c++; }
	if (x + half < size){ avg += coord(x + half, y); c++; }
	if (y - half > 0){ avg += coord(x, y - half); c++; }
	if (x - half > 0){ avg += coord(x - half, y); c++; }

	if (c != 0){
		avg /= c;
		coord(x, y) = avg;
		//coord(x, y) += offset;
	}
	coord(x, y) += offset;

}


void TerrainGen::makeIndices(){
	vector<unsigned int> indices;

	//for each sq in the mesh
	for (int k = 0; k < size - 1; k++){ // y
		for (int i = 0; i < size - 1; i++){ // x
			float TB_diag = fabs(coordVec(i, k) - coordVec(i + 1, k + 1));
			float BT_diag = fabs(coordVec(i, k + 1) - coordVec(i + 1, k));

			//wind clockwise
			//[] = y*size + x
			if (BT_diag < TB_diag){//    | / |
				indices.push_back((size*(k)) + i);
				indices.push_back((size*(k + 1)) + i);
				indices.push_back((size*(k)) + i + 1);


				indices.push_back((size*(k + 1)) + i);
				indices.push_back((size*(k + 1)) + i + 1);
				indices.push_back((size*(k)) + i + 1);
			}

			else{//      | \ |
				indices.push_back((size*(k)) + i);
				indices.push_back((size*(k + 1)) + i + 1);
				indices.push_back((size*(k)) + i + 1);


				indices.push_back((size*(k)) + i);
				indices.push_back((size*(k + 1)) + i);
				indices.push_back((size*(k + 1)) + i + 1);
			}


		}
	}

	individualIndexSize = indices.size();

	if (index == 0u){
		glGenBuffers(1, &index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* individualIndexSize, indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)* individualIndexSize, indices.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void TerrainGen::draw(){

}


void TerrainGen::drawFromMap(){

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);

	glBindBuffer(GL_ARRAY_BUFFER, surfaceMesh.VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, surfaceMesh.NORM_BUFF_ID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawElements(GL_TRIANGLES, individualIndexSize, GL_UNSIGNED_INT, 0);
}

#ifdef DEBUG_ADDITIONAL
void TerrainGen::drawLines(Shader* linesShader){
	glUseProgram(linesShader->theProgram);

	glEnableVertexAttribArray(0); // position

	glBindBuffer(GL_ARRAY_BUFFER, debugNormLines.VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glDrawArrays(GL_LINES, 0, 2*size*size);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glUseProgram(0);
}
#endif

void TerrainGen::genIndivNormals(){
	float separation = width / ((float)(size - 1));

	surfaceMesh.normals.reserve(size*size);
	surfaceMesh.normals.resize(size*size);
#ifdef DEBUG_ADDITIONAL
	debugNormLines.verticies.reserve(size*size * 2);
	debugNormLines.verticies.resize(size*size * 2);
#endif

	//ignore the edges for now 
	for (int x = 0; x < size; x++){
		surfaceMesh.normals[((0)*size + x)] = vector3(0.0f, 1.0f, 0.0f);
		surfaceMesh.normals[((size-1)*size + x)] = vector3(0.0f, 1.0f, 0.0f);
	}
	for (int y = 0; y < size; y++){
		surfaceMesh.normals[((y)*size + 0)] = vector3(0.0f, 1.0f, 0.0f);
		surfaceMesh.normals[((y)*size + size - 1)] = vector3(0.0f, 1.0f, 0.0f);
	}

	//inner
	for (int x = 1; x < size - 1; x++){
		for (int y = 1; y < size - 1; y++){
			vector3 norm, accumulate;
			accumulate.zero();
			vector3 v1, v2;
			//UPPER RIGHT QUADRANT
			float Bot_Top = fabs(coord(x, y) - coord(x + 1, y - 1));
			float Top_Bot = fabs(coord(x, y-1) - coord(x + 1, y));

			if (Bot_Top < Top_Bot){//       | / |       //consider lesseining the influence of doubles
				v1 = vector3(separation, coord(x, y) - coord(x + 1, y), 0.0f);
				v2 = vector3(separation, coord(x, y) - coord(x + 1, y - 1), -separation);
				norm = v1 % v2;
				norm.normalize();
				accumulate += (norm  ); // += 0.9f*norm
				v1 = vector3(0.0f, coord(x, y) - coord(x, y - 1), -separation);
				norm = v2 % v1;
				norm.normalize();
				accumulate += (norm  ); // += 0.9f*norm
			}
			else{ //           |\            //
			v1 = vector3(separation, coord(x, y) - coord(x + 1, y), 0.0f);
			v2 = vector3(0.0f, coord(x, y) - coord(x, y - 1), -separation);
			norm = v1 % v2;
			norm.normalize();
			accumulate += norm;
			}


			//UPPER LEFT QUADRANT
			Bot_Top = fabs(coord(x - 1, y) - coord(x, y - 1));
			Top_Bot = fabs(coord(x - 1, y - 1) - coord(x, y));

			if (Bot_Top < Top_Bot){//        / |.      
				v1 = vector3(0.0f, coord(x, y) - coord(x, y - 1), -separation);
				v2 = vector3(-separation, coord(x, y) - coord(x - 1, y), 0.0f);
				norm = v1 % v2;
				norm.normalize();
				accumulate += norm; //
			
			}
			else{ //           |\|            //
				v1 = vector3(0.0f, coord(x, y) - coord(x, y - 1), -separation);
				v2 = vector3(-separation, coord(x, y) - coord(x - 1, y - 1), -separation);
				norm = v1 % v2;
				norm.normalize();
				accumulate += (norm  );
				v1 = vector3(-separation, coord(x, y) - coord(x - 1, y), 0.0f);
				norm = v2 % v1;
				norm.normalize();
				accumulate += (norm  );
			}

			//LOWER LEFT QUADRANT
			Bot_Top = fabs(coord(x, y) - coord(x - 1, y + 1));
			Top_Bot = fabs(coord(x - 1, y) - coord(x, y + 1));

			if (Bot_Top < Top_Bot){//        | / |
				v1 = vector3(-separation, coord(x, y) - coord(x - 1, y), 0.0f);
				v2 = vector3(-separation, coord(x, y) - coord(x - 1, y + 1), separation);
				norm = v1 % v2;
				norm.normalize();
				accumulate += (norm  );
				v1 = vector3(0.0f, coord(x, y) - coord(x, y + 1), separation);
				norm = v2 % v1;
				norm.normalize();
				accumulate += (norm  );
			}
			else{//        \|'
				v1 = vector3(-separation, coord(x, y) - coord(x - 1, y), 0.0f);
				v2 = vector3(0.0f, coord(x, y) - coord(x, y + 1), separation);
				norm = v1 % v2;
				norm.normalize();
				accumulate += norm;
			}

			//LOWER RIGHT QUADRANT
			Bot_Top = fabs(coord(x, y + 1) - coord(x + 1, y));
			Top_Bot = fabs(coord(x, y) - coord(x + 1, y + 1));

			if (Bot_Top < Top_Bot){//     '|/          //
				v1 = vector3(0.0f, coord(x, y) - coord(x, y + 1), separation);
				v2 = vector3(separation, coord(x, y) - coord(x + 1, y), 0.0f);
				norm = v1 % v2;
				norm.normalize();
				accumulate += norm;
			}
			else{ //         '| \ |
				v1 = vector3(0.0f, coord(x, y) - coord(x, y + 1), separation);
				v2 = vector3(separation, coord(x, y) - coord(x + 1, y + 1), separation);
				norm = v1 % v2;
				norm.normalize();
				accumulate += (norm  );

				v1 = vector3(separation, coord(x, y) - coord(x + 1, y), 0.0f);
				norm = v2 % v1;
				norm.normalize();
				accumulate += (norm  );

			}


			//accumulate *= -1.0f;
			accumulate.normalize();
			surfaceMesh.normals[((y)*size + x)] = accumulate;

#ifdef DEBUG_ADDITIONAL
			debugNormLines.verticies[((y)*size + x) * 2] = vector3(x*separation + chunkCoords.first*width, coord(x, y), y * separation + chunkCoords.second*width);
			debugNormLines.verticies[((y)*size + x) * 2 + 1] = vector3(x*separation - 3.0f*accumulate[0] + chunkCoords.first*width
				, coord(x, y) + 3.0f*accumulate[1], y * separation - 3.0f*accumulate[2] + chunkCoords.second*width);
#endif 
		}// y
	}// x


	vector<float> guassKern;
	myMath::getGuassKern(&guassKern, 1);
	myMath::convolveVectorSqSepKern(&surfaceMesh.normals, size, guassKern, guassKern);
	for each (vector3 norm in surfaceMesh.normals)
	{
		norm.normalize();
	}



	surfaceMesh.genNormBuffer();


#ifdef DEBUG_ADDITIONAL
	debugNormLines.genVertBuffer();
#endif
}

