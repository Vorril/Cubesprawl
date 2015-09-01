#pragma once

class vector2
{
public:
typedef vector2 vec2;
	vector2();
	vector2(float u, float v);
	//~vector2();

	float coords[2];

	vector2 operator+(const vector2& add){
		return vector2(this->coords[0] + add.coords[0], this->coords[1] + add.coords[1]);
	}
};

