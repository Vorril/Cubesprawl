#pragma once

#include "GL_Loader.h"


class WorldObject
{
public:
static Shader* diffuseTexShader;
static Shader* litTexShader;
static Shader* litTexPVMShader;

static matrix4 const* cameraPV;//not the greatest encapsulation/flexability
//static matrix4 const* cameraV;
//static matrix4 const* cameraP;

	WorldObject();
	~WorldObject();

	vec4VBO* mesh;
	GLuint texture;

	modelMat m_Matrix;
	matrix4 rotMat;
	float scale;
	vec3 worldTranslation;

	//TODO Library or load semantics
	void loadMesh(const char* File);

	void loadTexture(const char* File, bool flagMips = true, bool compress = false);

	//fully assemble m, from scale rot trans
	void assemble_M();

	void draw()const;

	void drawLit()const;

	//draw with only m ready ie multiply in pv is neccassary
	void drawLitPVM()const;
};

