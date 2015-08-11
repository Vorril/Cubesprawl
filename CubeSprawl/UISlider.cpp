#include "UISlider.h"
#include <iostream>

UISlider::UISlider()
{
}
UISlider::UISlider(int slideIndex, int buttonIndex, point offset, dims dimensions){
	slIndex = slideIndex;
	buIndex = buttonIndex;

	//button dims done in uiframe b/c of math
	button = UISliderButton(buIndex);
}
UISlider::UISlider(int slideIndex, int buttonIndex, point offset, dims dimensions, void(*callBackFunc)()){
	slIndex = slideIndex;
	buIndex = buttonIndex;

	//button dims done in uiframe b/c of math
	button = UISliderButton(buIndex);

	setCallback(callBackFunc);
}

UISlider::~UISlider()
{
}

void UISlider::activate(float startMouseX){
	prevMouseX = startMouseX;
	button.activate();
}

void UISlider::drag(float xCoord){


	//could consider offset so drag button doesnt auto center


	if (xCoord > (float)(position.first + dimensions.first)){
		xCoord = (float)(position.first + dimensions.first);
	}
	else if (xCoord < position.first){
		xCoord = (float)position.first;
	}
	

	*slVar = slLowVal + slValWidth*((xCoord - (float)position.first) / (float)dimensions.first);

	button.translateTo(xCoord);

	//// if(associateCB)
	button.slideCallback();

}

float UISlider::buttonCenter(){
	return button.offset.first + (button.dimensions.first / 2);
}
void UISlider::setCallback(void(*callBackFunc)()){
	this->button.setCallBack(callBackFunc);
}
  /////////////////////////////////////
 ////// SLIDER BUTTON //////////////// ----[]----------
/////////////////////////////////////

//press and activate are the same thing for this inheritance
void UISliderButton::press(){
	activate();
}
void UISliderButton::release(){
	moveListening = false;
}
void UISliderButton::activate(){
	moveListening = true;
}
void UISliderButton::translate(float deltaX){
	offset.first += deltaX;
}
void UISliderButton::translateTo(float xCoord){
	offset.first = xCoord - (dimensions.first / 2);
}


UISliderButton::UISliderButton(){

}
UISliderButton::UISliderButton(int bIndex){
	index = bIndex;

	callBack = &defaultCB;//replace with new UISB semantics
}
UISliderButton::~UISliderButton(){

}

//Sliders will only sometimes want an assoicated function
//This could be handled with activate() BUT rather that handles the movement
//Bad example of inheritence probably
void UISliderButton::slideCallback(){
	callBack();
}