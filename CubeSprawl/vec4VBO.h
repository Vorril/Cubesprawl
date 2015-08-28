#pragma once
/*
A class to store and handle the data and only the data of an object
wrap inside a higher order object
Use:
Fill the buffers by passing this object to modelLoader.h then call the genFunctions
draw functions are mostly being phased out of this class

genTex includes genUV

doesnt need to be called vec4VBO really just VBO
*/
#include <matrix4.h> //includes vec4.h and math.h
#include <vector3.h>
#include <vector2.h>
#include <vector>


#include <Shader.h>//glew
//#include <GL\SOIL.h>//can remove if vbo generation is entirely abstracted I think it might still be used directly 

#include <iostream>//temporary

using std::vector;

class vec4VBO
{
public:
	//Members/////////////////////////////////////////////////////
	vector<vector4> verticies;
	vector<float> normals;//stands out not being a vec3...
	vector<vector2> texCoords;
	vector<vector3> texCoords_3D;

	//Generic // here a layer index
	vector<float> index;

	GLuint VAO_ID;//curr unused

	GLuint VERT_BUFF_ID;
	GLuint UV_BUFF_ID; 
	GLuint UVW_BUFF_ID;
	GLuint NORM_BUFF_ID;
	GLuint INDEX_BUFF_ID;


	//Construction and cleanup/////////////////////////////////////////////////////
	vec4VBO();
	~vec4VBO();
	//completely clean when done
	void cleanup();
	//clear but still plan on using (ie save VBO IDs)
	void clear();

	//OGL buffering methods/////////////////////////////////////////////////////
	//Generate for the first time
	void genVertBuffer(unsigned int hint = GL_STATIC_DRAW);
	//Regen after a resize
	void regenVertBuffer(unsigned int hint = GL_DYNAMIC_DRAW);
	//Update after change of data (but not resize)
	void refreshVertBuffer();
	void genUVBuffer();
	//Generate for the first time
	void genNormBuffer(unsigned int hint = GL_STATIC_DRAW);
	//Regen after a resize
	void refreshNormBuffer();
	//Update after change of data (but not resize)
	void regenNormBuffer();
	void regenUVBuffer();
	void refreshUVBuffer();
	
	void genIndexBuffer(unsigned int hint = GL_STATIC_DRAW);
	void regenIndexBuffer(unsigned int hint = GL_DYNAMIC_DRAW);
	void refreshIndexBuffer();

	void genUVWBuffer(unsigned int hint = GL_STATIC_DRAW);
	void regenUVWBuffer(unsigned int hint = GL_STATIC_DRAW);
	void refreshUVWBuffer();

	void add(float xAdd, float yAdd, float zAdd);
	void add(float xAdd, float yAdd, float zAdd, float wAdd);
	void add(vector4 vecAdd);
	void add(vector3 vecAdd);


	void addUV(float rAdd, float sAdd);

	void addUVW(float uAdd, float vAdd, float wAdd);
	

	void addNorm(vector3 normal);
	void addNorm(float x, float y, float z);



	//For use with small non pvm meshes for static movement
	void translate(float xAdd, float yAdd, float zAdd);

	vector4 & operator[](int i);

	int size()const{
		return verticies.size();
	}
};

