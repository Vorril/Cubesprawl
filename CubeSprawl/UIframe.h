#pragma once
#include <Shader.h>

#include <UIButton.h>
#include <UISlider.h>
#include <UITextBox.h>

#include <GL/glew.h>
#include <vector>

/*
Top level control for UI elements
Add interactive elements through an instance of this
Tell the frame mouse events/position and it will distribute to children accordingly

TODO: strongly consider making movement of elements and or zoom a shader responsibility
*/
class UIframe
{
public:
	static Shader flatShader;
	////////////////////////////

	UIframe();
	~UIframe();

	//vbo of quads////////////
	GLuint VBO_ID;
	vector<vector3> verticies;
	//////////////////////////

	//In this basic UI everything is essentially squares
	// Each square gets an index so coords = 4*index + n(1:4) etc
	// Buttons get 1 index
	// sliders get 2 indicies
	int indexCount = 0;

	vector<UIButton> buttons;
	vector<UISlider> sliders;

	void addButton(point offset, dims dimension, void(*callBack)());
	void addButton(point offset, dims dimension);

	//try the parameterObject design pattern sometime
	void addSlider(point offset, dims dimension, float* varf, float lowVal, float hiVal);
	void addSlider(point offset, dims dimension, float* varf, float lowVal, float hiVal, void(*callBackFunc)());

	void checkUIelementOverlap(point off, dims dimens);

	// Interactions ///////////
	///////////////////////////
	int currentMouseover = -1;
	// Pass mousecoords to anything that might be listening, assign currMouseover if found something
	// -->> FEED IT GL COORDS: return std::make_pair(2.0*mouseX/(width-1) - 1.0, -(2.0*mouseY/(height-1) - 1.0));
	// All buttons Listen
	// Sliders only listen while being held
	void mouseOver(point mouse);
	void clickTest(point clicked, int action);

	void draw();
	void draw(Shader* shaderToUse);

	//Call this on startup
	void toggleActive(){
		frameActive = !frameActive;
	}

	
	
private:
	void initializeUI();

	void initializeVBO();
	//when elements have changed but the amount is the same
	void refreshVBO();
	//when the number of elements has increased
	void remakeVBO();

	bool frameActive = true;
	
};

