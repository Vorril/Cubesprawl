#pragma once

#include <Text.h>
#include <Shader.h>

#include <GL/glew.h>

#include <vector3.h>
#include <vec4VBO.h>

/*
Holds text object(s)
Positions them
Draw either HUD, billboard, or in world
*/

class UITextBox
{
public:
	enum TEXTBOX_TYPES{HUD, WORLD, BILLBOARD};//may only do thwe first two, and may split classes at that


	UITextBox();
	UITextBox(TEXTBOX_TYPES type);
	~UITextBox();

	Text* font;

	string savedText;
	float scale = 20.0f; // arbitrary
	float width = 15.0f; // characters wide (in kernUnits)
	float charPos = 0.0f; // Position in the line (in kernUnits)
	int line = 0;

	void setPosition(vector3 pos);
	void setPosition(float x, float y, float z);

	//for only world
	void setFace(vector3 right, vector3 down);

	void addText(char* words);
	//Will not scale// hieght not exact based on input
	void addTextPixelSized(char* words, int charheight, int winWidth, int winHeight, int lineWidth = 400);

	void setFont(char* file);

	void draw();
	bool visible = true;

	void clear();
private:
	TEXTBOX_TYPES boxType;
	void initializeShader(TEXTBOX_TYPES);

	static Shader HUDTextShader;
	static Shader pvTextShader;
	//static Shader billboardTextShader;

	vec4VBO dataBuffers;

	vector3 position;
	vector3 direction;// horz
	vector3 down; 
	
};
