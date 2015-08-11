#pragma once
#include <utility>
#include <vector3.h>
//consider vec4 transparency

namespace UIelements{

typedef std::pair<double, double> point;


typedef std::pair<double, double> dims;
}


	using namespace UIelements;

class UIButton{
public:
	enum BUTTON_STATUS{NUETRAL, OVER, DOWN};

	UIButton();
	UIButton(point position, dims size);
	UIButton(point position, dims size, void(*callBack)());
	~UIButton();

	//index in the VBO
	int index = 0;

	//TL corner
	point offset;
	//Width, height
	dims dimensions;

	vector3 colorNuetral = vector3(0.4f, 0.3f, 0.3f);
	vector3 colorOver = vector3(0.0f, 0.7f, 0.3f);
	vector3 colorDown = vector3(0.8f, 0.1f, 0.15f);
	vector3 getColor();


	void setCallBack(void(*callBackFunc)());
	
	//could potentially have CBs for all these
	void mouseOver();
	void mouseOff();
	void press();
	void release();
	BUTTON_STATUS getStatus();

	//deprecated//debug purposes
	void activate();

	bool hitTest(point hitTest);
protected:
	static void defaultCB();
	void(*callBack)();

	BUTTON_STATUS status = NUETRAL;
};

