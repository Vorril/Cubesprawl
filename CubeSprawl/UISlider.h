#pragma once
#include <UIButton.h>

//active component of a slider
class UISliderButton : public UIButton{
public:
	UISliderButton(int bIndex);
	UISliderButton();
	~UISliderButton();

	void press();
	void release();
	void activate();


//is being dragged
	bool moveListening = false;

	void translate(float deltaX);
	void translateTo(float xCoord);

	void slideCallback();

	
private:
};

/*
1) Listen for mouse down
2) Start requesting mouseCoordinates / movement
2.2) Listen for mouse up
3) Stop on mouse up

*/
class UISlider
{
public:
	UISlider();
	UISlider(int slideIndex, int buttonIndex, point offset, dims dimensions);
	UISlider(int slideIndex, int buttonIndex, point offset, dims dimensions, void(*callBackFunc)());
	~UISlider();

	int slIndex; 
	UISliderButton button;
	int buIndex;

	point position;
	dims dimensions;

	vector3 slideCol = vector3(0.4f, 0.4f, 0.4f);
	vector3 buttonCol = vector3(0.5f, 0.3f, 0.57f);

	float slLowVal;
	float slValWidth;
	float* slVar;

	void drag(float xCoord);
	void activate(float startMouseX);

	float prevMouseX;

	float buttonCenter();

	void setCallback(void(*callBackFunc)());
};


