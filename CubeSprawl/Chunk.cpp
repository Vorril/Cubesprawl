#include "Chunk.h"


Chunk::Chunk() :Chunk(-1)
{
}

Chunk::Chunk(int height)
{
	for (int y = 0; y < 16; y++){
		for (int x = 0; x < 16; x++){
			chunkStacks[x][y] = new CubeStack(height);
		}
	}

	height == -1 ? this->height = 16 : this->height = height;

	buffer.genVertBuffer();
	buffer.genUVWBuffer();
	buffer.genIndexBuffer();
}

Chunk::~Chunk()
{
	for (int y = 0; y < 16; y++){
		for (int x = 0; x < 16; x++){
			delete chunkStacks[x][y];
		}
	}

	buffer.cleanup();
}


//Dont use
void Chunk::bufferAll(){
	buffer.clear();


for (int x = 0; x < 16; x++){
		for (int z = 0; z < 16; z++){
			//CubeStack * const stack = chunkStacks[x][z];
			CubeStack& stack = *chunkStacks[x][z];
			for (int y = 0; y < stack.lengthAvail; y++){//if perfect could be length filled and no null check
				if (stack.stack[y] != nullptr){
					float xf = (16.0f)*chunkX + (float)x;
					float zf = (16.0f)*chunkY + (float)z;
					float yf = (float)y;
				
					int type = stack.stack[y]->getType();


					const float baseX = 0.0f; 
					const float baseY = 0.0f;
					const float UVunit = 1.0f;

					//thirty verts for 10 tris (currently no bottoms)
					//Probably a more efficient way to do this
					for (int i = 0; i < 36; i++){
						buffer.index.push_back((float)type);
					}

					// Top ////
					buffer.add(xf + 1.0f, yf + 1.0f, zf); //BR
					buffer.add(xf,        yf + 1.0f, zf); //BL
					buffer.add(xf,        yf + 1.0f, zf + 1.0f);//TL
					buffer.add(xf,        yf + 1.0f, zf + 1.0f);//TL
					buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TR
					buffer.add(xf + 1.0f, yf + 1.0f, zf);//BR
					
					buffer.addUV(baseX + UVunit, baseY);//BR
					buffer.addUV(baseX, baseY);//BL
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX + UVunit, baseY + UVunit);//TR
					buffer.addUV(baseX + UVunit, baseY);//BR

					// Bot ////
					buffer.add(xf + 1.0f, yf, zf);       //BR
					buffer.add(xf,        yf, zf + 1.0f);//TL
					buffer.add(xf,        yf, zf);       //BL
					buffer.add(xf,        yf, zf + 1.0f);//TL
					buffer.add(xf + 1.0f, yf, zf);       //BR
					buffer.add(xf + 1.0f, yf, zf + 1.0f);//TR

					buffer.addUV(baseX + UVunit, baseY);//BR
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX, baseY);//BL
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX + UVunit, baseY);//BR
					buffer.addUV(baseX + UVunit, baseY + UVunit);//TR


					// Left ////
					buffer.add(xf + 1.0f, yf + 1.0f, zf);//TL
					buffer.add(xf + 1.0f, yf, zf); //BL
					buffer.add(xf, yf, zf); //BR
					buffer.add(xf + 1.0f, yf + 1.0f, zf);//TL
					buffer.add(xf, yf, zf);//BR
					buffer.add(xf, yf + 1.0f, zf);//TR

					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX, baseY);//BL
					buffer.addUV(baseX + UVunit, baseY);//BR
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX + UVunit, baseY);//BR
					buffer.addUV(baseX + UVunit, baseY + UVunit);//TR

					// Right ////
					buffer.add(xf, yf, zf + 1.0f); //BR
					buffer.add(xf + 1.0f, yf, zf + 1.0f); //BL
					buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TL
					buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TL
					buffer.add(xf, yf + 1.0f, zf + 1.0f);//TR
					buffer.add(xf, yf, zf + 1.0f);//BR

					buffer.addUV(baseX + UVunit, baseY);//BR
					buffer.addUV(baseX, baseY);//BL
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX, baseY + UVunit);//TL
					buffer.addUV(baseX + UVunit, baseY + UVunit);//TR
					buffer.addUV(baseX + UVunit, baseY);//BR


					// Front ////
					buffer.add(xf, yf, zf);
					buffer.add(xf, yf, zf + 1.0f);
					buffer.add(xf, yf + 1.0f, zf + 1.0f);
					buffer.add(xf, yf, zf);
					buffer.add(xf, yf + 1.0f, zf + 1.0f);
					buffer.add(xf, yf + 1.0f, zf);

					buffer.addUV(baseX + 1.0f, baseY);//BR
					buffer.addUV(baseX, baseY);//BL
					buffer.addUV(baseX, baseY + 1.0f);//TL
					buffer.addUV(baseX + 1.0f, baseY);//BR
					buffer.addUV(baseX, baseY + 1.0f);//TL
					buffer.addUV(baseX + 1.0f, baseY + 1.0f);//TR

					// Back ////
					buffer.add(xf + 1.0f, yf, zf);
					buffer.add(xf + 1.0f, yf + 1.0f, zf);
					buffer.add(xf + 1.0f, yf, zf + 1.0f);
					buffer.add(xf + 1.0f, yf + 1.0f, zf);
					buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);
					buffer.add(xf + 1.0f, yf, zf + 1.0f);

					buffer.addUV(baseX, baseY);//BL
					buffer.addUV(baseX, baseY + 1.0f);//TL
					buffer.addUV(baseX + 1.0f, baseY);//BR
					buffer.addUV(baseX, baseY + 1.0f);//TL
					buffer.addUV(baseX + 1.0f, baseY + 1.0f);//TR
					buffer.addUV(baseX + 1.0f, baseY);//BR


				}

			}//y
		}//z
	}//x
	
	//This is for first time buffering
	//A different function should be used in order to
	buffer.regenVertBuffer();
	buffer.regenUVWBuffer();
	buffer.regenIndexBuffer();

	needsBuffered = false;
}

void Chunk::bufferAllVisible(){
	buffer.clear();

	list<Cube*>::iterator iter;
	for (iter = exposedCubes.begin(); iter != exposedCubes.end(); iter++){
		
		float xf = (float)((*iter)->x);
		float zf = (float)((*iter)->z);
		float yf = (float)(*iter)->y;

		float type = (float)(*iter)->getType();
		
		const float baseX = 0.0f;
		const float baseY = 0.0f;
		const float UVunit = 1.0f;

		//thirty verts for 10 tris (currently no bottoms)
		//Probably a more efficient way to do this
		

		// Top ////
		buffer.add(xf + 1.0f, yf + 1.0f, zf); //BR
		buffer.add(xf, yf + 1.0f, zf); //BL
		buffer.add(xf, yf + 1.0f, zf + 1.0f);//TL
		buffer.add(xf, yf + 1.0f, zf + 1.0f);//TL
		buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TR
		buffer.add(xf + 1.0f, yf + 1.0f, zf);//BR

		buffer.addUVW(baseX + UVunit, baseY, type);//BR
		buffer.addUVW(baseX, baseY, type);//BL
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR
		buffer.addUVW(baseX + UVunit, baseY, type);//BR

		// Bot ////
		buffer.add(xf + 1.0f, yf, zf);       //BR
		buffer.add(xf, yf, zf + 1.0f);//TL
		buffer.add(xf, yf, zf);       //BL
		buffer.add(xf, yf, zf + 1.0f);//TL
		buffer.add(xf + 1.0f, yf, zf);       //BR
		buffer.add(xf + 1.0f, yf, zf + 1.0f);//TR

		buffer.addUVW(baseX + UVunit, baseY, type);//BR
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX, baseY, type);//BL
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX + UVunit, baseY, type);//BR
		buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR

		// Left ////
		buffer.add(xf + 1.0f, yf + 1.0f, zf);//TL
		buffer.add(xf + 1.0f, yf, zf); //BL
		buffer.add(xf, yf, zf); //BR
		buffer.add(xf + 1.0f, yf + 1.0f, zf);//TL
		buffer.add(xf, yf, zf);//BR
		buffer.add(xf, yf + 1.0f, zf);//TR

		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX, baseY, type);//BL
		buffer.addUVW(baseX + UVunit, baseY, type);//BR
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX + UVunit, baseY, type);//BR
		buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR

		// Right ////
		buffer.add(xf, yf, zf + 1.0f); //BR
		buffer.add(xf + 1.0f, yf, zf + 1.0f); //BL
		buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TL
		buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TL
		buffer.add(xf, yf + 1.0f, zf + 1.0f);//TR
		buffer.add(xf, yf, zf + 1.0f);//BR

		buffer.addUVW(baseX + UVunit, baseY, type);//BR
		buffer.addUVW(baseX, baseY, type);//BL
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX, baseY + UVunit, type);//TL
		buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR
		buffer.addUVW(baseX + UVunit, baseY, type);//BR


		// Front ////
		buffer.add(xf, yf, zf);
		buffer.add(xf, yf, zf + 1.0f);
		buffer.add(xf, yf + 1.0f, zf + 1.0f);
		buffer.add(xf, yf, zf);
		buffer.add(xf, yf + 1.0f, zf + 1.0f);
		buffer.add(xf, yf + 1.0f, zf);

		buffer.addUVW(baseX + 1.0f, baseY, type);//BR
		buffer.addUVW(baseX, baseY, type);//BL
		buffer.addUVW(baseX, baseY + 1.0f, type);//TL
		buffer.addUVW(baseX + 1.0f, baseY, type);//BR
		buffer.addUVW(baseX, baseY + 1.0f, type);//TL
		buffer.addUVW(baseX + 1.0f, baseY + 1.0f, type);//TR

		// Back ////
		buffer.add(xf + 1.0f, yf, zf);
		buffer.add(xf + 1.0f, yf + 1.0f, zf);
		buffer.add(xf + 1.0f, yf, zf + 1.0f);
		buffer.add(xf + 1.0f, yf + 1.0f, zf);
		buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);
		buffer.add(xf + 1.0f, yf, zf + 1.0f);

		buffer.addUVW(baseX, baseY, type);//BL
		buffer.addUVW(baseX, baseY + 1.0f, type);//TL
		buffer.addUVW(baseX + 1.0f, baseY, type);//BR
		buffer.addUVW(baseX, baseY + 1.0f, type);//TL
		buffer.addUVW(baseX + 1.0f, baseY + 1.0f, type);//TR
		buffer.addUVW(baseX + 1.0f, baseY, type);//BR

	}//iterate loop
			

	buffer.regenVertBuffer();
	buffer.regenUVWBuffer();

	needsBuffered = false;
}

void Chunk::bufferAllVisibleSides(){
	buffer.clear();

	list<Cube*>::iterator iter;
	for (iter = exposedCubes.begin(); iter != exposedCubes.end(); iter++){

		float xf = (float)((*iter)->x);
		float zf = (float)((*iter)->z);
		float yf = (float)(*iter)->y;

		float type = (float)(*iter)->getType();

		const float baseX = 0.0f;
		const float baseY = 0.0f;
		const float UVunit = 1.0f;

		const int code = (*iter)->exposedSides;

		

			// Top ////
		if (code & 1){
			buffer.add(xf + 1.0f, yf + 1.0f, zf); //BR
			buffer.add(xf, yf + 1.0f, zf); //BL
			buffer.add(xf, yf + 1.0f, zf + 1.0f);//TL
			buffer.add(xf, yf + 1.0f, zf + 1.0f);//TL
			buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TR
			buffer.add(xf + 1.0f, yf + 1.0f, zf);//BR

			buffer.addUVW(baseX + UVunit, baseY, type);//BR
			buffer.addUVW(baseX, baseY, type);//BL
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR
			buffer.addUVW(baseX + UVunit, baseY, type);//BR

			/*for (int i = 0; i < 6; i++){
				buffer.index.push_back(type);
			}*/
		}

		// Bot ////
		if (code & 2){
			buffer.add(xf + 1.0f, yf, zf);       //BR
			buffer.add(xf, yf, zf + 1.0f);//TL
			buffer.add(xf, yf, zf);       //BL
			buffer.add(xf, yf, zf + 1.0f);//TL
			buffer.add(xf + 1.0f, yf, zf);       //BR
			buffer.add(xf + 1.0f, yf, zf + 1.0f);//TR

			buffer.addUVW(baseX + UVunit, baseY, type);//BR
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX, baseY, type);//BL
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX + UVunit, baseY, type);//BR
			buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR

		}

		// Left ////
		if (code & 4){
			buffer.add(xf + 1.0f, yf + 1.0f, zf);//TL
			buffer.add(xf + 1.0f, yf, zf); //BL
			buffer.add(xf, yf, zf); //BR
			buffer.add(xf + 1.0f, yf + 1.0f, zf);//TL
			buffer.add(xf, yf, zf);//BR
			buffer.add(xf, yf + 1.0f, zf);//TR

			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX, baseY, type);//BL
			buffer.addUVW(baseX + UVunit, baseY, type);//BR
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX + UVunit, baseY, type);//BR
			buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR

		}

		// Right ////
		if (code & 8){
			buffer.add(xf, yf, zf + 1.0f); //BR
			buffer.add(xf + 1.0f, yf, zf + 1.0f); //BL
			buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TL
			buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);//TL
			buffer.add(xf, yf + 1.0f, zf + 1.0f);//TR
			buffer.add(xf, yf, zf + 1.0f);//BR

			buffer.addUVW(baseX + UVunit, baseY, type);//BR
			buffer.addUVW(baseX, baseY, type);//BL
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX, baseY + UVunit, type);//TL
			buffer.addUVW(baseX + UVunit, baseY + UVunit, type);//TR
			buffer.addUVW(baseX + UVunit, baseY, type);//BR

			
		}

		// Back ////
		if (code & 16){
			buffer.add(xf, yf, zf);
			buffer.add(xf, yf, zf + 1.0f);
			buffer.add(xf, yf + 1.0f, zf + 1.0f);
			buffer.add(xf, yf, zf);
			buffer.add(xf, yf + 1.0f, zf + 1.0f);
			buffer.add(xf, yf + 1.0f, zf);

			buffer.addUVW(baseX + 1.0f, baseY, type);//BR
			buffer.addUVW(baseX, baseY, type);//BL
			buffer.addUVW(baseX, baseY + 1.0f, type);//TL
			buffer.addUVW(baseX + 1.0f, baseY, type);//BR
			buffer.addUVW(baseX, baseY + 1.0f, type);//TL
			buffer.addUVW(baseX + 1.0f, baseY + 1.0f, type);//TR

			
		}

		// Forward ////
		if (code & 32){
			buffer.add(xf + 1.0f, yf, zf);
			buffer.add(xf + 1.0f, yf + 1.0f, zf);
			buffer.add(xf + 1.0f, yf, zf + 1.0f);
			buffer.add(xf + 1.0f, yf + 1.0f, zf);
			buffer.add(xf + 1.0f, yf + 1.0f, zf + 1.0f);
			buffer.add(xf + 1.0f, yf, zf + 1.0f);

			buffer.addUVW(baseX, baseY, type);//BL
			buffer.addUVW(baseX, baseY + 1.0f, type);//TL
			buffer.addUVW(baseX + 1.0f, baseY, type);//BR
			buffer.addUVW(baseX, baseY + 1.0f, type);//TL
			buffer.addUVW(baseX + 1.0f, baseY + 1.0f, type);//TR
			buffer.addUVW(baseX + 1.0f, baseY, type);//BR

			
		}

	}//iterate loop


	buffer.regenVertBuffer();
	buffer.regenUVWBuffer();

	needsBuffered = false;
}

int Chunk::getChunkType(int x, int y, int z)const{
	return chunkStacks[x][z]->stack[y]->type;
}

Cube* Chunk::getChunkCube(int x, int y, int z){
	return chunkStacks[x][z]->stack[y];//unsure
}

//Put in chunk coords ie 0-15 ie world coords & 15
Cube** Chunk::getCubePosition(int x, int y, int z)const{
	if (y > chunkStacks[x][z]->lengthAvail-1) return nullptr;//note - 1 for indexing
	return &(chunkStacks[x][z]->stack[y]);
}


Cube* Chunk::occupied(int x, int y, int z)const{
	return chunkStacks[x][z]->stack[y];
}

void Chunk::hide(int x, int y, int z){
	hide(chunkStacks[x][z]->stack[y]);
}
void Chunk::hide(Cube* noLongerVisable){
	exposedCubes.remove(noLongerVisable);
	needsBuffered = true;
}

void Chunk::reveal(int x, int y, int z){
	reveal(chunkStacks[x][z]->stack[y]);
}
void Chunk::reveal(Cube* nowVisable){
	exposedCubes.push_back(nowVisable);
	needsBuffered = true;
}

void Chunk::draw()const{
	glBindBuffer(GL_ARRAY_BUFFER, buffer.VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.UVW_BUFF_ID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 2));


	glDrawArrays(GL_TRIANGLES, 0, buffer.verticies.size());

}

void Chunk::draw(Shader* texShader, GLuint tex)const{
}

void Chunk::drawToFBO()const{
	glBindBuffer(GL_ARRAY_BUFFER, buffer.VERT_BUFF_ID);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.verticies.size());
}