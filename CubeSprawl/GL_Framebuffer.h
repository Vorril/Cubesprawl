#pragma once

#include <GL/glew.h>

class GL_Framebuffer
{
public:
	GL_Framebuffer();
	GL_Framebuffer(int width, int height);
	~GL_Framebuffer();

	GLuint FB_ID;
	GLuint COLOR_ID;//color attachment
	GLuint DEPTH_ID;//depth attachment
	GLuint TEX_ID;//Texture to render to

	void bind()const;
	void unbind()const;
private:
};

