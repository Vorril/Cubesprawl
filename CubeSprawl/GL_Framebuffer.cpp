#include "GL_Framebuffer.h"


GL_Framebuffer::GL_Framebuffer():GL_Framebuffer(720,1280) {

}

GL_Framebuffer::GL_Framebuffer(int width, int height){
	glGenFramebuffersEXT(1, &FB_ID);
	bind();

	glGenRenderbuffersEXT(1, &COLOR_ID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, COLOR_ID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, width, height);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, COLOR_ID);

	glGenRenderbuffersEXT(1, &DEPTH_ID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DEPTH_ID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, DEPTH_ID);

	unbind();
}

GL_Framebuffer::~GL_Framebuffer()
{
}


void GL_Framebuffer::bind()const{
	glBindFramebufferEXT(FB_ID, GL_FRAMEBUFFER);//GL_FRAMEBUFFER = Read/Write // GL_READ/DRAW_FRAMEBUFFER exist
}

void GL_Framebuffer::unbind()const{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}