#include "vec4VBO.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////CONSTRUCTORS////////////////
//////////////////////////////////////////////////////////////////
vec4VBO::vec4VBO()
{
}


vec4VBO::~vec4VBO()
{
}

void vec4VBO::cleanup(){
	glDeleteBuffers(1, &VERT_BUFF_ID);
	glDeleteBuffers(1, &UV_BUFF_ID);
	glDeleteBuffers(1, &UVW_BUFF_ID);
	glDeleteBuffers(1, &NORM_BUFF_ID);
	
	verticies.clear(); verticies.shrink_to_fit();//warning on shrinktofit not guaranteed
	normals.clear(); normals.shrink_to_fit();
	texCoords.clear(); texCoords.shrink_to_fit();
	texCoords_3D.clear(); texCoords_3D.shrink_to_fit();

}

void vec4VBO::clear(){
	if (VERT_BUFF_ID != 0u){
		verticies.clear(); 
	}
	if (UV_BUFF_ID != 0u){
		texCoords.clear();
	}
	if (UVW_BUFF_ID != 0u){
		texCoords_3D.clear();
	}
	if (NORM_BUFF_ID != 0u){
		normals.clear();
	}
	if (INDEX_BUFF_ID != 0u){
		index.clear();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// OPEN_GL BUFFERING METHODS ///////////////
//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////
///// VERTICIES /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

//Call after filling
//upload the vertex data to the GPU
void vec4VBO::genVertBuffer(unsigned int hint){
	glGenBuffers(1, &VERT_BUFF_ID); // most important 
	glBindBuffer(GL_ARRAY_BUFFER, VERT_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float) * verticies.size(),
		verticies.data(),
		hint);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//call this when the vbo has grown in length and refresh when it has not
void vec4VBO::regenVertBuffer(unsigned int hint){
	glBindBuffer(GL_ARRAY_BUFFER, VERT_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float)* verticies.size(),
		verticies.data(),
		hint);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//Call when updating, could be private
//Only updates verts
void vec4VBO::refreshVertBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, VERT_BUFF_ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float) * verticies.size(),
		verticies.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// NORMS ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

//Fill first, uptades norms to the GPU
void vec4VBO::genNormBuffer(unsigned int hint){
	glGenBuffers(1, &NORM_BUFF_ID);
	glBindBuffer(GL_ARRAY_BUFFER, NORM_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * normals.size(),
		normals.data(),
		hint);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::refreshNormData(){
	glBindBuffer(GL_ARRAY_BUFFER, NORM_BUFF_ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float)* normals.size(),
		normals.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::regenNormBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, NORM_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)* normals.size(),
		normals.data(),
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///// TEX COORDS /////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void vec4VBO::genUVBuffer(){
	glGenBuffers(1, &UV_BUFF_ID); // most important 
	glBindBuffer(GL_ARRAY_BUFFER, UV_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * texCoords.size(),
		texCoords.data(),
		GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::regenUVBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, UV_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * texCoords.size(),
		texCoords.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void vec4VBO::refreshUVBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, UV_BUFF_ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(float) * texCoords.size(),
		texCoords.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// TEX COORDS  3D /////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vec4VBO::genUVWBuffer(unsigned int hint){
	glGenBuffers(1, &UVW_BUFF_ID); // most important 
	glBindBuffer(GL_ARRAY_BUFFER, UVW_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * texCoords_3D.size(),
		texCoords_3D.data(),
		hint);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::regenUVWBuffer(unsigned int hint ){
	glBindBuffer(GL_ARRAY_BUFFER, UVW_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * texCoords_3D.size(),
		texCoords_3D.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::refreshUVWBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, UVW_BUFF_ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * texCoords_3D.size(),
		texCoords_3D.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// INDEX ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void vec4VBO::genIndexBuffer(unsigned int hint){
	glGenBuffers(1, &INDEX_BUFF_ID); // most important 
	glBindBuffer(GL_ARRAY_BUFFER, INDEX_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * index.size(),
		index.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::regenIndexBuffer(unsigned int hint){
	glBindBuffer(GL_ARRAY_BUFFER, INDEX_BUFF_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * index.size(),
		index.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vec4VBO::refreshIndexBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, INDEX_BUFF_ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * index.size(),
		index.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////METHODS & OPERATIONS//////////////
//////////////////////////////////////////////////////////////////
void vec4VBO::add(vector4 vecAdd){
	this->verticies.push_back(vecAdd);
}
void vec4VBO::add(float xAdd, float yAdd, float zAdd){
	this->add(vector4(xAdd, yAdd, zAdd));
}

void vec4VBO::add(vector3 vecAdd){
	add(vector4(vecAdd[0], vecAdd[1], vecAdd[2]));
}

void vec4VBO::add(float xAdd, float yAdd, float zAdd, float wAdd){
	add(vector4(xAdd, yAdd, zAdd, wAdd));
}


void vec4VBO::addUV(float rAdd, float sAdd){
	this->texCoords.push_back(vector2(rAdd, sAdd));
}

void vec4VBO::addUVW(float u, float v, float w){
	texCoords_3D.push_back(vector3(u, v, w));
}

vector4 &vec4VBO::operator[](int i){
	return this->verticies[i];
}

void vec4VBO::translate(float xAdd, float yAdd, float zAdd){
	const vector3 translation(xAdd, yAdd, zAdd);

	for (int i = 0; i < verticies.size(); i++){
		verticies[i] += translation;
	}

	refreshVertBuffer();
}