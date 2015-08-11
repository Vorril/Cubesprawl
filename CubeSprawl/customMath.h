#pragma once

#include <vector>
#include <matrix4.h> //includes vec4, mat3, vec3, 

namespace myMath{

	//convolve a square with a seperable (also square) kernel
	inline void convolveSqSepKern(float* matrix, int sqSize, std::vector<float> &vKern, std::vector<float> &hKern){
	
		float accumulate = 0;
		int kernSize = vKern.size();
		int kernRadius = kernSize / 2; 

		float* firstPass = new float[sqSize*sqSize];

		for (int x = kernRadius; x< sqSize-kernRadius; x++){
			for (int y = kernRadius; y < sqSize-kernRadius; y++){
				for (int i = 0; i < kernSize; i++){
					if (y - kernRadius + i >= 0 && y - kernRadius + i < sqSize)// SIDE MANAGEMENT
						accumulate += vKern[i] * matrix[x + ( (y - kernRadius + i)*sqSize)];
					
				}
				firstPass[((y)*sqSize + x)] = accumulate;
				accumulate = 0;
			}
		}
		
		for (int y = kernRadius; y < sqSize - kernRadius; y++){
			for (int x = 1+kernRadius; x< sqSize -1-kernRadius; x++){//// hKern is sampling the edges of firstPass which were never set!
				for (int i = 0; i < kernSize; i++){
					if (x - kernRadius + i >= 0 && x - kernRadius + i < sqSize)// SIDE MANAGEMENT
							accumulate += hKern[i] * firstPass[((x - kernRadius + i) + (y*sqSize))];
				}
				
				matrix[((y)*sqSize + x)] = accumulate;
				accumulate = 0;
			}
		}


		delete[] firstPass;

	}

	inline void convolveVectorSqSepKern(vector<vector3>* matrix, int sqSize, std::vector<float> &vKern, std::vector<float> &hKern){

		vector3 accumulate;
		int kernSize = vKern.size();
		int kernRadius = kernSize / 2;

		vector<vector3> firstPass;
		firstPass.reserve(sqSize*sqSize);
		firstPass.resize(sqSize*sqSize);

		for (int x = kernRadius; x< sqSize - kernRadius; x++){
			for (int y = kernRadius; y < sqSize - kernRadius; y++){
				for (int i = 0; i < kernSize; i++){
					if (y - kernRadius + i >= 0 && y - kernRadius + i < sqSize)// SIDE MANAGEMENT
						accumulate +=  (*matrix)[x + ((y - kernRadius + i)*sqSize)] * vKern[i];

				}
				firstPass[((y)*sqSize + x)] = (accumulate);
				accumulate.zero();
			}
		}

		for (int y = kernRadius; y < sqSize - kernRadius; y++){
			for (int x = 1 + kernRadius; x< sqSize - 1 - kernRadius; x++){//// hKern is sampling the edges of firstPass which were never set!
				for (int i = 0; i < kernSize; i++){
					if (x - kernRadius + i >= 0 && x - kernRadius + i < sqSize)// SIDE MANAGEMENT
						accumulate += firstPass[((x - kernRadius + i) + (y*sqSize))] * hKern[i];
				}

				(*matrix)[((y)*sqSize + x)] = accumulate;
				accumulate.zero();
			}
		}



	}

	//size = radius ie 1 is a 3 kern// 2 is a 5 kern
	inline void getGuassKern(std::vector<float>* fillMe, int size = 1){
		float a = 1.5f;
		float recipCoeff = 1.0f / (a* 2.50663f);
		float expCoeff = -1.0f / (2 * a*a);

		float sum = 0.0f;
		for (int i = -size; i <= size; i++){
			fillMe->push_back(recipCoeff * expf(i*i*expCoeff));
			sum += fillMe->back();
		}

		for (int i = 0; i < fillMe->size(); i++){ //size() should = 2*br + 1
			//fillMe->at(i) /= sum; 
			(*fillMe)[i] /= sum;// normalize
		}


	}




}//namsepace