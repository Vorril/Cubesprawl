#include "UIButton.h"
#include <iostream>

UIButton::UIButton(){
	callBack = &defaultCB;
}

UIButton::UIButton(point position, dims size){
	offset = position;
	dimensions = size;
	callBack = &defaultCB;
}

UIButton::UIButton(point position, dims size, void(*callBackFunc)()){
	offset = position;
	dimensions = size;
	callBack = callBackFunc;
}

UIButton::~UIButton()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool UIButton::hitTest(point hit){
	return hit.first > offset.first && hit.second > offset.second &&
		hit.first < (offset.first + dimensions.first) && hit.second < (offset.second + dimensions.second);
}

void UIButton::mouseOver(){
	if (status != DOWN)
	status = OVER;
}
void UIButton::mouseOff(){
	status = NUETRAL;
}
void UIButton::press(){
	status = DOWN;
}
void UIButton::release(){
	callBack();
	status = NUETRAL;
}
vector3 UIButton::getColor(){
	switch (status)
	{
	case UIButton::NUETRAL:
		return colorNuetral;
	case UIButton::OVER:
		return colorOver;
	case UIButton::DOWN:
		return colorDown;
	default:
		break;
	}
	return vector3(1.0f, 1.0f, 1.0f);
}

UIButton::BUTTON_STATUS UIButton::getStatus(){
	return status;
}

void UIButton::setCallBack(void(*callBackFunc)()){
	callBack = callBackFunc;
}

void UIButton::activate(){
	callBack();
}

void UIButton::defaultCB(){
	std::cout << "Default cb" << std::endl;
}