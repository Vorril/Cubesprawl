#include "CubeStack.h"


CubeStack::CubeStack() :CubeStack(-1)
{
}

CubeStack::CubeStack(int length){

	if (length != -1){
		lengthAvail = length;//default 16
		stack.resize(length);
	}

	else{
		//lengthAvail = length;//default 16
		stack.resize(lengthAvail);
	}
}

CubeStack::~CubeStack()
{
	
	for (int i = 0; i < lengthAvail; i++){
		if(stack[i] != nullptr) delete stack[i];
	}
	
}

