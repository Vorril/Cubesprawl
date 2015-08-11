#include "UITextBox.h"


Shader UITextBox::HUDTextShader = Shader();
Shader UITextBox::pvTextShader = Shader();

UITextBox::UITextBox()
{
}


UITextBox::UITextBox(TEXTBOX_TYPES type){
	if (!visible) return;

	switch (type)
	{
	case UITextBox::HUD:
		boxType = type;
		if (!UITextBox::HUDTextShader.initialized)
			initializeShader(type);
		scale = 0.05f;
		direction = vector3(1.0f, 0.0f, 0.0f);
		down = vector3(0.0f, -1.0f, 0.0f);
		break;
	case UITextBox::WORLD:
		boxType = type;
		if (!UITextBox::pvTextShader.initialized)
			initializeShader(type);
		break;
	case UITextBox::BILLBOARD:
		break;
	default:
		break;
	}
}

UITextBox::~UITextBox()
{
	if (font != nullptr)
		delete font;
}

void UITextBox::initializeShader(TEXTBOX_TYPES type){
	switch (type)
	{
	case UITextBox::HUD:
		UITextBox::HUDTextShader.initialized = true;
		UITextBox::HUDTextShader.InitializeProgram("Shaders/Text/hudTextShader.vert", "Shaders/Text/hudTextShader.frag");
		UITextBox::HUDTextShader.samplerIndex1 = glGetUniformLocation(UITextBox::HUDTextShader.theProgram, "textHudSampler");
		glGenSamplers(1, &UITextBox::HUDTextShader.sampler1);
		glSamplerParameteri(UITextBox::HUDTextShader.sampler1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(UITextBox::HUDTextShader.sampler1, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case UITextBox::WORLD:
		UITextBox::pvTextShader.initialized = true;
		UITextBox::pvTextShader.InitializeProgram("Shaders/Text/pvTextShader.vert", "Shaders/Text/pvTextShader.frag");
		UITextBox::pvTextShader.samplerIndex1 = glGetUniformLocation(UITextBox::pvTextShader.theProgram, "textSampler");
		glGenSamplers(1, &UITextBox::pvTextShader.sampler1);
		glSamplerParameteri(UITextBox::pvTextShader.sampler1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(UITextBox::pvTextShader.sampler1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		pvTextShader.linkUBO();
		break;
	case UITextBox::BILLBOARD:
		break;
	default:
		break;
	}
	dataBuffers.genVertBuffer();
	dataBuffers.genUVBuffer();
	//color//dataBuffers.genNormBuffer();
}

void UITextBox::setFont(char* file){
	//TODO: consider reassignment risks dangling pointers
	font = new Text(file);
}

void UITextBox::setPosition(vector3 pos){
	charPos = 0.0f;
	line = 0;
	position = pos;
}
void UITextBox::setPosition(float x, float y, float z){
	setPosition(vector3(x, y, z));
}

//for only world
void UITextBox::setFace(vector3 right, vector3 down){
	right.normalize();
	down.normalize();

	direction = right;
	this->down = down;
}

void UITextBox::draw(){
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);


	Shader* shaderToUse = nullptr;
	switch (boxType)
	{
	case UITextBox::HUD:
		shaderToUse = &UITextBox::HUDTextShader;
		break;
	case UITextBox::WORLD:
		shaderToUse = &UITextBox::pvTextShader;
		break;
	case UITextBox::BILLBOARD:
		break;
	default:
		return;
		break;
	}
	glUseProgram(shaderToUse->theProgram);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // uv tex coords

	glUniform1i(shaderToUse->samplerIndex1, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font->texture);
	glBindSampler(0, shaderToUse->sampler1);


	glBindBuffer(GL_ARRAY_BUFFER, dataBuffers.VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, dataBuffers.UV_BUFF_ID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, dataBuffers.verticies.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void UITextBox::addText(char* words){
	string adding = string(words);
	savedText += adding;

	for (unsigned int i = 0; i < adding.size(); i++){
		if (charPos >= width){//flow to next line
			charPos = 0.0f;
			line++;
			//consider triStrip using redundant verts here
		}

		vector2 UV = font->getCoord(adding[i]);

		float xUnit = 1.0f / font->charsWide;
		float yUnit = 1.0f / font->charsTall;

		float kernFactor = font->getKernFactor(adding[i]);

		//clockwise winding
		dataBuffers.verticies.push_back(position + (down*scale*line) + (direction*scale*charPos));//TL
		dataBuffers.verticies.push_back(position + (down*scale*line) + (direction*scale*(charPos + kernFactor)));//TR
		dataBuffers.verticies.push_back(position + (down*scale*(line+1)) + (direction*scale*charPos));//BL

		dataBuffers.verticies.push_back(position + (down*scale*line) + (direction*scale*(charPos + kernFactor)));//TR
		dataBuffers.verticies.push_back(position + (down*scale*(line + 1)) + (direction*scale*(charPos + kernFactor)));//BR
		dataBuffers.verticies.push_back(position + (down*scale*(line + 1)) + (direction*scale*charPos));//BL

		dataBuffers.texCoords.push_back(UV);
		dataBuffers.texCoords.push_back(UV + vector2(xUnit*kernFactor, 0.0f));
		dataBuffers.texCoords.push_back(UV + vector2(0.0f, yUnit));

		dataBuffers.texCoords.push_back(UV + vector2(xUnit*kernFactor, 0.0f));
		dataBuffers.texCoords.push_back(UV + vector2(xUnit*kernFactor, yUnit));
		dataBuffers.texCoords.push_back(UV + vector2(0.0f, yUnit));


		charPos+= kernFactor;
	}//for each char

		dataBuffers.regenVertBuffer();
		dataBuffers.regenUVBuffer();

}


void UITextBox::addTextPixelSized(char* words, int charHeight, int winWidth, int winHeight, int lineWidth){
	string adding = string(words);
	savedText += adding;

	//these two should already be true
	direction = vector3(1.0f, 0.0f, 0.0f);
	down = vector3(0.0f, -1.0f, 0.0f);

	float aspect = (float)winWidth / winHeight;
	float xScale, yScale; 
	//glUnitsPer Pixel
	xScale = (2.0f / winWidth);//GLU per pixel
	yScale = (2.0f / winHeight);
	float widthGLU = xScale*lineWidth;
	float charWidthGLU = xScale*charHeight;//charHeight=charWidth square text units
	float charHeightGLU = yScale*charHeight;

		float xUnit = 1.0f / font->charsWide;
		float yUnit = 1.0f / font->charsTall;

	for (unsigned int i = 0; i < adding.size(); i++){
		if ((charPos+1)*charWidthGLU >= widthGLU){//flow to next line
			charPos = 0.0f;
			line++;
			//consider triStrip using redundant verts here
		}

		vector2 UV = font->getCoord(adding[i]);

		float kernFactor = font->getKernFactor(adding[i]);

		//clockwise winding
		//Down and Direction are unit vectors
		//charPos and line are in kernUnits (character space in a way)
		//The final part is the scaling
		dataBuffers.verticies.push_back(position + (down*charHeightGLU*line) + (direction*charWidthGLU*charPos));//TL
		dataBuffers.verticies.push_back(position + (down*charHeightGLU*line) + (direction*charWidthGLU*(charPos + kernFactor)));//TR
		dataBuffers.verticies.push_back(position + (down*charHeightGLU*(line + 1)) + (direction*charWidthGLU*charPos));//BL

		dataBuffers.verticies.push_back(position + (down*charHeightGLU*line) + (direction*charWidthGLU*(charPos + kernFactor)));//TR
		dataBuffers.verticies.push_back(position + (down*charHeightGLU*(line + 1)) + (direction*charWidthGLU*(charPos + kernFactor)));//BR
		dataBuffers.verticies.push_back(position + (down*charHeightGLU*(line + 1)) + (direction*charWidthGLU*charPos));//BL

		dataBuffers.texCoords.push_back(UV);
		dataBuffers.texCoords.push_back(UV + vector2(xUnit*kernFactor, 0.0f));
		dataBuffers.texCoords.push_back(UV + vector2(0.0f, yUnit));

		dataBuffers.texCoords.push_back(UV + vector2(xUnit*kernFactor, 0.0f));
		dataBuffers.texCoords.push_back(UV + vector2(xUnit*kernFactor, yUnit));
		dataBuffers.texCoords.push_back(UV + vector2(0.0f, yUnit));


		charPos += kernFactor;//char pos is in kernUnits
	}//for each char

	dataBuffers.regenVertBuffer();
	dataBuffers.regenUVBuffer();
}

void UITextBox::clear(){
	dataBuffers.clear();
}