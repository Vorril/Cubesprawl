#include "UIframe.h"

Shader UIframe::flatShader = Shader();

UIframe::UIframe(){
	if (!flatShader.initialized){
		initializeUI();
		flatShader.initialized = true;
	}
	initializeVBO();//for background boxes or something // currently unused
}

UIframe::~UIframe()
{
	glDeleteShader(flatShader);
}

void UIframe::addButton(point offset, dims dimension, void(*callBack)()){
	buttons.push_back(UIButton(offset, dimension, callBack));
	buttons.back().index = indexCount++;

	verticies.push_back(vector3((float)offset.first, (float)offset.second));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)offset.second));
	verticies.push_back(vector3((float)offset.first, (float)(offset.second + dimension.second)));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)(offset.second + dimension.second)));

	remakeVBO();
}
void UIframe::addButton(point offset, dims dimension){
	buttons.push_back(UIButton(offset, dimension));
	buttons.back().index = indexCount++;

	verticies.push_back(vector3((float)offset.first, (float)offset.second));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)offset.second));
	verticies.push_back(vector3((float)offset.first, (float)(offset.second + dimension.second)));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)(offset.second + dimension.second)));

	remakeVBO();
}

void UIframe::addSlider(point offset, dims dimension, float* varf, float lowVal, float hiVal){
	int slideIndex = indexCount++;
	int buttonIndex = indexCount++;
	sliders.push_back(UISlider(slideIndex, buttonIndex, offset, dimension));

	// the slider bar comes first
	verticies.push_back(vector3((float)offset.first, (float)offset.second));
	verticies.push_back(vector3((float)(offset.first + (float)dimension.first), (float)offset.second));
	verticies.push_back(vector3((float)offset.first, (float)(offset.second + (float)dimension.second)));
	verticies.push_back(vector3((float)(offset.first + (float)dimension.first), (float)(offset.second + (float)dimension.second)));

	sliders.back().dimensions = dimension;
	sliders.back().position = offset;

	//now the slider button
	if (lowVal > hiVal) swap(hiVal, lowVal);//dont know what swap actually does just assuming

	float buttonWidth = (float)dimension.first / 8.0f;
	float buttonHeight = (float)dimension.second*1.2f;
	float posHorz = (*varf-lowVal) / (hiVal - lowVal);//portion slided to begin out of 1.0 // (5-2)/(8-2) 1 {2 3 4 |5| 6 7 8}
	offset.first += (posHorz*(float)dimension.first - buttonWidth / 2);
	offset.second += (((float)dimension.second / 2) - (buttonHeight / 2));
	dimension.second = (double)buttonHeight;
	dimension.first = (double)buttonWidth;

	sliders.back().button.offset = offset;
	sliders.back().button.dimensions = dimension;
	sliders.back().slLowVal = lowVal;
	sliders.back().slValWidth = (hiVal - lowVal);
	sliders.back().slVar = varf;

	verticies.push_back(vector3((float)offset.first, (float)offset.second));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)offset.second));
	verticies.push_back(vector3((float)offset.first, (float)(offset.second + dimension.second)));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)(offset.second + dimension.second)));

	remakeVBO();
}

void UIframe::addSlider(point offset, dims dimension, float* varf, float lowVal, float hiVal, void(*callBackFunc)()){
	int slideIndex = indexCount++;
	int buttonIndex = indexCount++;
	sliders.push_back(UISlider(slideIndex, buttonIndex, offset, dimension, callBackFunc));

	// the slider bar comes first
	verticies.push_back(vector3((float)offset.first, (float)offset.second));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)offset.second));
	verticies.push_back(vector3((float)offset.first, (float)(offset.second + dimension.second)));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)(offset.second + dimension.second)));

	sliders.back().dimensions = dimension;
	sliders.back().position = offset;

	//now the slider button
	if (lowVal > hiVal) swap(hiVal, lowVal);//dont know what swap actually does just assuming

	float buttonWidth = dimension.first / 8.0f;
	float buttonHeight = dimension.second*1.2f;
	float posHorz = (*varf - lowVal) / (hiVal - lowVal);//portion slided to begin out of 1.0 // (5-2)/(8-2) 1 {2 3 4 |5| 6 7 8}
	offset.first += (posHorz*dimension.first - buttonWidth / 2);
	offset.second += ((dimension.second / 2) - (buttonHeight / 2));
	dimension.second = buttonHeight;
	dimension.first = buttonWidth;

	sliders.back().button.offset = offset;
	sliders.back().button.dimensions = dimension;
	sliders.back().slLowVal = lowVal;
	sliders.back().slValWidth = (hiVal - lowVal);
	sliders.back().slVar = varf;

	verticies.push_back(vector3((float)offset.first, (float)offset.second));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)offset.second));
	verticies.push_back(vector3((float)offset.first, (float)(offset.second + dimension.second)));
	verticies.push_back(vector3((float)(offset.first + dimension.first), (float)(offset.second + dimension.second)));

	remakeVBO();
}

void UIframe::checkUIelementOverlap(point off, dims dimens){

}

  //////////////////////////
 ///// INTERACTION ////////
//////////////////////////
void UIframe::mouseOver(point mouse){
	if (!frameActive) return;

	for (UISlider &slider : sliders){
		if (slider.button.moveListening){
			//no longer over the button, dragged too far maybe
			
			slider.drag(mouse.first);
			
			verticies[slider.buIndex*4 + 0]=(vector3((float)slider.button.offset.first, (float)slider.button.offset.second));
			verticies[slider.buIndex*4 + 1]=(vector3((float)(slider.button.offset.first + slider.button.dimensions.first), (float)slider.button.offset.second));
			verticies[slider.buIndex*4 + 2]=(vector3((float)slider.button.offset.first, (float)(slider.button.offset.second + slider.button.dimensions.second)));
			verticies[slider.buIndex*4 + 3]=(vector3((float)(slider.button.offset.first + slider.button.dimensions.first), (float)(slider.button.offset.second + slider.button.dimensions.second)));

			refreshVBO();
			return;//nothing else possible, cant drag and click
		}
	}

	for (UIButton &button : buttons){
		if (!button.hitTest(mouse)){
			if (button.index == currentMouseover){//missed but was prev over
				currentMouseover = -1;
				button.mouseOff();
			}
		}//failed test
		else{//hit
			button.mouseOver();
			currentMouseover = button.index;
		}
	}//buttons


}

void UIframe::clickTest(point clicked, int action){
	if (!frameActive) return;

	if (action == 1){ //down
		for (UIButton &button : buttons){
			if (button.hitTest(clicked))
				button.press();
		}
		for (UISlider &slider : sliders){
			if (slider.button.hitTest(clicked)) 
				slider.activate(clicked.first);
		}
	}
	else if (action == 0){//release
		for (UIButton &button : buttons){
			if (button.hitTest(clicked) && button.getStatus() == UIButton::BUTTON_STATUS::DOWN)
				button.release();
		}
		for (UISlider &slider : sliders){
			if (slider.button.moveListening) 
				slider.button.release();
		}
	}
}

void UIframe::draw(){
	if (!frameActive) return;

	glUseProgram(flatShader.theProgram);

	//Bind the verticies buffer
	glEnableVertexAttribArray(0); // position
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	vector3 colorToUse;
	for (UIButton &button : buttons){//get rid of these for each
		
		colorToUse = button.getColor();
		glUniform3f(flatShader.uniform1, colorToUse.coords[0], colorToUse.coords[1], colorToUse.coords[2]);
		glDrawArrays(GL_TRIANGLE_STRIP, button.index * 4, 4);
	}
	for (UISlider &slider : sliders){
		colorToUse = slider.buttonCol;
		glUniform3f(flatShader.uniform1, colorToUse.coords[0], colorToUse.coords[1], colorToUse.coords[2]);
		glDrawArrays(GL_TRIANGLE_STRIP, slider.buIndex * 4, 4);

		colorToUse = slider.slideCol;
		glUniform3f(flatShader.uniform1, colorToUse.coords[0], colorToUse.coords[1], colorToUse.coords[2]);
		glDrawArrays(GL_TRIANGLE_STRIP, slider.slIndex * 4, 4);

		
	}


	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

void UIframe::draw(Shader* shaderToUse){
	glUseProgram(shaderToUse->theProgram);
	//glUniform3f(shaderToUse->uniform1, 1.0f, 0.0f, 0.0f);

	//Bind the verticies buffer
	glEnableVertexAttribArray(0); // position
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	for (int i = 0; i < buttons.size(); i++){
		glDrawArrays(GL_TRIANGLE_STRIP, buttons[i].index * 4, 4);
	}

	glDisableVertexAttribArray(0);
	glUseProgram(0);
}




// Private //////
////////////////////////////////////
void UIframe::initializeUI(){
	UIframe::flatShader.InitializeProgram("Shaders/flatColor.vert", "Shaders/flatColor.frag");
	glUseProgram(UIframe::flatShader.theProgram);
	UIframe::flatShader.uniform1 = glGetUniformLocation(UIframe::flatShader.theProgram, "color");
	glUniform3f(UIframe::flatShader.uniform1, 1.0f, 0.0f, 0.0f);
	glUseProgram(0);
}

void UIframe::initializeVBO(){
	glGenBuffers(1, &VBO_ID); // most important 
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)* verticies.size(),
		verticies.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//when elements have changed but the amount is the same
void UIframe::refreshVBO(){
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float)* verticies.size(),
		verticies.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//when the number of elements has increased
void UIframe::remakeVBO(){
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)* verticies.size(),
		verticies.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}