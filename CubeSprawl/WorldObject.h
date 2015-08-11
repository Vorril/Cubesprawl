#pragma once

#include "GL_Loader.h"


class WorldObject
{
public:
static Shader* diffuseTexShader;
static Shader* litTexShader;

	WorldObject();
	~WorldObject();

	vec4VBO* mesh;
	GLuint texture;

	//TODO Library or load semantics
	void loadMesh(const char* File);

	void loadTexture(const char* File, bool flagMips = true, bool compress = false);

	void draw()const;

	void drawLit()const;
};

