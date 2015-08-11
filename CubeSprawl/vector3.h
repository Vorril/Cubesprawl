#pragma once
#include <GL/glew.h>

class vector3
{
public:
	static const vector3 UNIT_X;
	static const vector3 UNIT_Y;
	static const vector3 UNIT_Z;

	vector3();
	vector3(float x, float y, float z);
	vector3(float x, float y);
	~vector3();

	float coords[3];

	void normalize();
	float fastInvSqrt(float number);
	void zero();

	vector3 operator-();
	vector3 operator-()const{ 
		return vector3(-coords[0], -coords[1], -coords[2]); //this worked
		//return -*this;
	}
	vector3 operator-(const vector3 &sub);

	vector3 operator+(const vector3& add);
	void operator+=(const vector3& add);

	float operator*(const vector3& dot);
	vector3 operator*(const float factor);
	vector3 cross(vector3 cross)const;
	vector3 operator%(const vector3 &cross);

	void operator*=(const float factor){
		this->coords[0] *= factor;
		this->coords[1] *= factor;
		this->coords[2] *= factor;
	}

	float operator[](int i) const{ return coords[i]; }
	float & operator[](int i){ return coords[i]; }
};

