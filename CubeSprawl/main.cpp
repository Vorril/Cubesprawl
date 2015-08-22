#include <iostream>
#include <GL/glew.h>//using the d library nd dll
#include <GL/glfw3.h> // includes windows.h
#include <GL/SOIL.h>

#include "GL_Loader.h"
#include "GL_Framebuffer.h"
#include "Shader.h"
#include "Camera.h"
#include "vec4VBO.h"

#include "Chunk.h"
#include "World.h"
#include "WorldBuilder.h"
#include "WorldObject.h"
#include "TerrainGen.h"
#include "UIFrame.h"

#define toggle(x) x=!x;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// PROTO FUNCTIONS ///////////
////////////////////////////////////////////////////////////////
void checkMouse();


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// CONSTANTS & VARIABLES ///////////
////////////////////////////////////////////////////////////////
Camera worldCam;

//Shaders////////////////////////////////
Shader flatCol;
Shader pvFlatCol;
Shader pvTexture;
Shader pvArrTexture;
//Shader pvColorFBO;
Shader pvLitTex;
Shader pvLitTexVary;
Shader hudText;
/////////////////////////////////////////

vec4VBO testNorms;

//Chunk* testChunk[10];
World& world = *(World::getInstance());//singleton
WorldBuilder testBuilder;

UIframe* UI;

WorldObject tower;

GLuint texArray;

bool bound = false;
bool paused = false;
bool mousedown = false;
bool lightControl = false;
float speedMod = 0.29f;

float tempf = 0.01f; float timef = 0.0f;
float diffuse = 0.30f;
float lambert = 0.70f;

vector4 light;

vec4VBO crosshair;
vec4VBO chunkDims;
UITextBox* textBox;

class windowData{ //Data for a window, width, height,  and mouse coords 
public:
	int width; int height;
	double mouseX;
	double mouseY;
	GLFWwindow* primaryWindow;
	float FoV = 56.0f;//degrees (vert)//these are windows properties should put them in the struct
	float HorzFoV = 0.0f;// a function of FoV(vert) and screen dims
	float degreesPerPixelVert = 1.0f;
	float degreesPerPixelHorz = 1.0f;//fov relation useful for mouse movement
	float mouseSensitivity = 1.2f;
	float dotCullLimit = 0.0f;
	void updateMouse(){
		glfwGetCursorPos(window.primaryWindow, &window.mouseX, &window.mouseY);//relative top left
	}
	point mousePosGLcoords()const{
		return std::make_pair(2.0*mouseX / (width - 1) - 1.0, -(2.0*mouseY / (height - 1) - 1.0)); //Whats up with those -1's
	}
	point arbPosGLcoords(int x, int y)const{
		return std::make_pair(2.0*x / (width - 1) - 1.0, -(2.0*y / (height - 1) - 1.0));
	}
	dims dimensionGLcoords(int w, int h)const{
		return std::make_pair(2.0*w / width, 2.0*h / height);
	}
}window;
GLFWmonitor* myMonitor = nullptr;

class interval{
public:
	double deltaT;
	double prevTime;

	void updateTime(){
		/*
		times.deltaT = glfwGetTime() - times.prevTime;
		//getTime ~ deltaT + prevTime
		//newPrevTime = getTime
		times.prevTime = times.deltaT + times.prevTime;//dont have to call getTime a second time, should be very close to true maybe better
		*/
		deltaT = glfwGetTime() - prevTime;
		prevTime += deltaT;
	}
}times;

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///////////////////
	// ENGINE ////////
	/////////////////
	times.updateTime();
	glfwPollEvents();
	checkMouse();

	bound ? worldCam.moveCamera(speedMod) : worldCam.moveCameraUnbound(speedMod);
	worldCam.update();

	//UI->mouseOver(window.mousePosGLcoords());
	////////////////\
	////////////////\\
	////////////////\\\





	///////////////////////////////
	//if (viewBounds){
	//	timef += times.deltaT;
	//	if (timef > 10.0f) timef -= 10.0f;
	//	light[0] = cosf(6.183f*timef / 10.0f);
	//	light[2] = sinf(6.183f*timef / 10.0f);
	//	Shader::updateUPO_light(light);
	//}
	//////////////////////////////


	world.draw();

	glUseProgram(flatCol.theProgram);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, crosshair.VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, 4);


	//glUseProgram(pvFlatCol);
	//glUniform3f(pvFlatCol.uniform1, 0.65f, 0.2f, 0.55f);
	//glBindBuffer(GL_ARRAY_BUFFER, testNorms.VERT_BUFF_ID);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//glDrawArrays(GL_TRIANGLES, 0, testNorms.size() );



	glUseProgram(0);


	tower.drawLit();


	//textBox->draw();

	//if (paused)
		//UI->draw();//TODO unify draws based on transparency, depth buffer clears, etc


	glfwSwapBuffers(window.primaryWindow);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////   CALLBACKS   //////////////
////////////////////////////////////////////////////////////////

void key_callback(GLFWwindow* windowP, int key, int scancode, int action, int mods){

	if (action == 1){//press 

		switch (key){
			//Movekeys:
		case 32://space 
			break;
		case 256://esc
			if (paused) { glfwSetInputMode(window.primaryWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }
			else { glfwSetInputMode(window.primaryWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
			paused = !paused;
			break;
		case 65://a
			worldCam.keyDownCode += 1;
			break;
		case 68://d
			worldCam.keyDownCode += 2;
			break;
		case 87://w
			worldCam.keyDownCode += 4;
			break;
		case 83://s
			worldCam.keyDownCode += 8;
			break;
		case 81://q
			worldCam.cameraPosition[1] += 1.0f;
			break;
		case 69://e
			worldCam.cameraPosition[1] -= 1.0f;
			break;
		case 340://lshift
			speedMod += 1.1f;
			break;
		case 341://lctrl
			mousedown = true;
			break;
		case 67:{//c
			

			double t1 = glfwGetTime();
			world.clear();
			testBuilder.setChunksAsTerrain(0, 0, 3, 3);
			cout << "Time Make: " << (glfwGetTime() - t1) * 1000 << "ms" << endl;
			break;
		}
		case 84:{//t
			/*double t1 = glfwGetTime();
			
			for each (Chunk* chunk in world.worldChunks){
				world.determineChunkExposed(chunk->chunkX, chunk->chunkY);
				chunk->bufferAllVisible();
			}
			cout << "Time All  : " << (glfwGetTime() - t1) * 1000 << "ms" << endl;


			t1 = glfwGetTime();
			for each (Chunk* chunk in world.worldChunks){
				world.determineChunkExposedSides(chunk->chunkX, chunk->chunkY);
				chunk->bufferAllVisibleSides();
			}
			cout << "Time Sided: " << (glfwGetTime() - t1) * 1000 << "ms" << endl;*/ 

			

			break;
		}
		case 66://b
			bound = !bound;
			break;
		case 78:{ //n
		}
				break;
		case 49:// 1
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 50:// 2
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 51:// 3
			toggle(lightControl);
			break;
		case 52:// 4
			if (WorldObject::litTexShader == &pvLitTex)
				WorldObject::litTexShader = &pvLitTexVary;
			else WorldObject::litTexShader = &pvLitTex;
			break;
		case 53:// 5
			break;
		case 54:// 6
			break;
		case 55:// 7
			break;
		case 56://8

			break;
		case GLFW_KEY_EQUAL:{// +
			break; }
		case GLFW_KEY_MINUS:{// -
			break;
		}
		case 71://g
			break;

		case 80:{//p
			break; }
		default:
			cout << key << endl;
			break;
		}//switch

	}//key was pressed >down<


	else if (action == 0){//release

		switch (key){

		case 65://a
			worldCam.keyDownCode -= 1;
			break;
		case 68://d
			worldCam.keyDownCode -= 2;
			break;
		case 87://w
			worldCam.keyDownCode -= 4;
			break;
		case 83://s
			worldCam.keyDownCode -= 8;
			break;
		case 340://lshift
			speedMod -= 1.1f;
			break;
		case 341://lctrl
			mousedown = false;
			break;

		default:{};
		}//switch

	}//key was released
}//keycall

void mouse_button_callback(GLFWwindow* windowP, int button, int action, int mods){
	UI->clickTest(window.mousePosGLcoords(), action);//tip the ui

	if (action == 1){
		mousedown = true;

		if (!paused){
			//First Person hittest
			int code = -1;
			Cube* test = world.clickTestSide(worldCam.cameraPosition, worldCam.lookDirection, code);
		
			if (test != nullptr){
				int x = test->x; int y = test->y; int z = test->z;
				//cout << x << ", " << y << ", " << z << endl;
				if (button == 1){
					testBuilder.killCube(x, y, z, true);
					testBuilder.setBuffers();
				}
				
				if (button == 0){
					
					switch (code)
					{
					case TOP:
						testBuilder.setCube(x, y + 1, z, RED, true);
						break;
					case BOT:
						testBuilder.setCube(x, y - 1, z, RED, true);
						break;
					case LEFT:
						testBuilder.setCube(x - 1, y, z, RED, true);
						break;
					case RIGHT:
						testBuilder.setCube(x + 1, y, z, RED, true);
						break;
					case FORW:
						testBuilder.setCube(x, y, z + 1, RED, true);
						break;
					case BACK:
						testBuilder.setCube(x, y, z - 1, RED, true);
						break;
					default:
						break;
					}

					testBuilder.setBuffers();
					
				}

			}

		}//!paused
		else{
			//Screen click hittest
			vector3 camLook = worldCam.lookDirection; camLook *= -1.0f;
			vector3 camPerp = worldCam.perpToLookDir;
			vector3 camUp = camPerp  % camLook ;

		
			float fov_rad = window.FoV * radiansPerDegree;
			float vLength = tanf(fov_rad / 2) * 0.1f;//0.1f == nearClipPlane
			float hLength = (vLength * window.width) / window.height;

			camUp = camUp*vLength;
			camPerp = camPerp*hLength;

			float x = (float)window.mouseX; 
			x -= (window.width / 2.0f);
			x /= (window.width / 2.0f);
			float y = window.height - (float)window.mouseY;
			y -= (window.height / 2.0f);
			y /= (window.height / 2.0f);

			vector3 rayDir = (camLook*0.1f) + (camPerp*x)+(camUp*y);
			rayDir.normalize();
			
			Cube* test = world.clickTest(worldCam.cameraPosition, rayDir*-1.0f);
			if (test != nullptr){
				int x = test->x; int y = test->y; int z = test->z;
				//cout << x << ", " << y << ", " << z << endl;
				if (button == 1){
					testBuilder.killCube(x, y, z);
					testBuilder.setBuffers();
				}

			}

		}//paused
	}//action1

	else{
		mousedown = false;

	}
};

void checkMouse(){
	window.updateMouse();
	if (!paused){
		if (!lightControl){//TEMP
			if (window.mouseX != window.width * 0.50 || window.mouseY != window.height * 0.50){
				double deltaX = window.width * 0.50 - window.mouseX;
				double deltaY = window.height * 0.50 - window.mouseY;
				glfwSetCursorPos(window.primaryWindow, window.width * 0.50, window.height * 0.50);
				//rotation fxns take degrees, then convert in matrix_.h
				worldCam.rotateCameraHorz(float(deltaX) * window.degreesPerPixelHorz);
				worldCam.rotateCameraVert(float(deltaY) * window.degreesPerPixelVert);
			}
		}

			else{//TEMP
				vector3 light3 = (vector3)light;
				double deltaX = window.width * 0.50 - window.mouseX;
				double deltaY = window.height * 0.50 - window.mouseY;
				glfwSetCursorPos(window.primaryWindow, window.width * 0.50, window.height * 0.50);
				float horzAngle = float(deltaX) * window.degreesPerPixelHorz;
				float vertAngle = float(deltaY) * window.degreesPerPixelVert;

				light3 = matrix3::makeRotateYaxis(-horzAngle)*light3;
				light3 = matrix3::makeRotateZaxis(vertAngle)*light3;
				light3.normalize();

				light = vector4(light3, light[3]);
				Shader::updateUPO_light(light);

			}//TEMP
		}//not paused
		else{
		}//paused
	
}

void scroll_callback(GLFWwindow* windowP, double xoffset, double yoffset){
	//window.degreesPerPixelHorz /= window.mouseSensitivity;
	//window.degreesPerPixelVert /= window.mouseSensitivity;
	//window.mouseSensitivity += (float)(yoffset *0.05);
	//window.degreesPerPixelHorz *= window.mouseSensitivity;
	//window.degreesPerPixelVert *= window.mouseSensitivity;

	light[3] += 0.1*yoffset;
	if (light[3] < 0.0) light[3] = 0.0;
	Shader::updateUPO_light(light);


}

void window_size_callback(GLFWwindow* windowP, int widthNew, int heightNew){
	window.width = widthNew;
	window.height = heightNew;
	glViewport(0, 0, widthNew, heightNew);

	worldCam.makeProjectionMatrix(window.FoV, (float)window.width / window.height, 0.1f, 2000.0f);



	/////Necc. updates:
	//for mouse movement:
	window.degreesPerPixelVert = (window.FoV / window.height);
	float d = (window.height / 2.0f) / tanf(window.FoV*radiansPerDegree / 2);
	float halfHorzFoV = atan((window.width / 2.0f) / d)*degreesPerRadian;
	window.HorzFoV = 2 * halfHorzFoV;//saving
	window.degreesPerPixelHorz = halfHorzFoV / (window.width / 2.0f);
	//For view culling:
	window.dotCullLimit = -cosf(window.HorzFoV*radiansPerDegree / 2);

	window.degreesPerPixelHorz *= window.mouseSensitivity;
	window.degreesPerPixelVert *= window.mouseSensitivity;
}


void tempCB_Top(){
	glUseProgram(pvLitTexVary.theProgram);
	glUniform1f(pvLitTexVary.uniform1, lambert);
	glUseProgram(0);
}
void tempCB_Bot(){
	glUseProgram(pvLitTexVary.theProgram);
	glUniform1f(pvLitTexVary.uniform2, diffuse);
	glUseProgram(0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////  SETUP & EXIT  //////////////
////////////////////////////////////////////////////////////////

void initialize(){
	/////////////////////////////////////////////////////////
	///////// Create Shaders ////////////////////////////////
	pvLitTex.InitializeProgram("Shaders/pvLitTex.vert", "Shaders/pvLitTex.frag");
	pvLitTexVary.InitializeProgram("Shaders/pvLitTexVary.vert", "Shaders/pvLitTexVary.frag");
	pvTexture.InitializeProgram("Shaders/pvTexture.vert", "Shaders/pvTexture.frag"); 
	flatCol.InitializeProgram("Shaders/flatColor.vert", "Shaders/flatColor.frag");
	pvFlatCol.InitializeProgram("Shaders/pvFlatColor.vert", "Shaders/pvFlatColor.frag");
	pvArrTexture.InitializeProgram("Shaders/pvArrTexture.vert", "Shaders/pvArrTexture.frag");
	//pvColorFBO.InitializeProgram("Shaders/pvColorFBO.vert", "Shaders/pvColorFBO.frag");

	pvArrTexture.setSampler("arrSampler", Shader::SAMPLER_SETTING::ANISTROPIC, 16);
	pvLitTex.setSampler("diffuseSampler", Shader::SAMPLER_SETTING::ANISTROPIC, 8);
	pvLitTexVary.setSampler("diffuseSampler", Shader::SAMPLER_SETTING::ANISTROPIC, 8);
	pvTexture.setSampler("diffuseSampler", Shader::SAMPLER_SETTING::ANISTROPIC, 8);


	flatCol.setUniform("color", 1);
	glUseProgram(flatCol.theProgram);
	glUniform3f(flatCol.uniform1, 0.1f, 0.8f, 0.2f);
	glUseProgram(0);
	pvFlatCol.setUniform("color", 1);
	glUseProgram(pvFlatCol.theProgram);
	glUniform3f(pvFlatCol.uniform1, 0.1f, 0.8f, 0.2f);
	glUseProgram(0);

	pvLitTexVary.setUniform("lambert", 1);
	pvLitTexVary.setUniform("diffuse", 2);
	glUseProgram(pvLitTexVary.theProgram);
	glUniform1f(pvLitTexVary.uniform1, lambert);
	glUniform1f(pvLitTexVary.uniform2, diffuse);
	glUseProgram(0);

	Shader::setGlobalPV_UBO(&pvLitTex);
	pvTexture.linkUBO();
	pvFlatCol.linkUBO();
	pvArrTexture.linkUBO();
	pvLitTexVary.linkUBO();

	// Light
	vector3 lightProperties(-1.0f, -0.4f, 1.0f);
	lightProperties.normalize();
	light = vector4(lightProperties);
	Shader::updateUPO_light(light);


	////////////////////////////////////////
	////Camera///////////////////

	worldCam = Camera(-0.64f, 0.52f, 0.56f, 5.0f, 9.0f, -2.0f);
	worldCam.makeProjectionMatrix(window.FoV, (float)window.width / window.height, 0.1f, 2000.0f);
	worldCam.update();

	window.degreesPerPixelVert = (window.FoV / window.height);
	float d = (window.height / 2.0f) / tanf(window.FoV*radiansPerDegree / 2);
	float halfHorzFoV = atan((window.width / 2.0f) / d)*degreesPerRadian;
	window.HorzFoV = 2 * halfHorzFoV;
	window.degreesPerPixelHorz = halfHorzFoV / (window.width / 2.0f);
	window.dotCullLimit = -cosf(window.HorzFoV*radiansPerDegree / 2);

	window.degreesPerPixelHorz *= window.mouseSensitivity;
	window.degreesPerPixelVert *= window.mouseSensitivity;

	///////////////////////////////////////
	////World////////////////////
	
	vector<string> files;
	files.push_back("Textures/HomeCube/home");
	//files.push_back("Textures/RedCube/red");
	files.push_back("Textures/red2");
	files.push_back("Textures/BlueCube/blue");

	GL_Loader::loadArrayTexture(texArray, 64, 64, 4, files,true);

	////////////////////////////////////////////////////////////////
	world.shader = &pvArrTexture;
	world.texArray = texArray;
	testBuilder.worldFocus = &world;

	////////////////////////////////////////////////////////////////


	testBuilder.setChunksAsTerrain(0, 0, 3, 3);

	/// UI ///
	///////////////////////////////
	UI = new UIframe();

	UI->addSlider(window.arbPosGLcoords(window.width - 180, 40), window.dimensionGLcoords(120, 35), &lambert, 0.0f, 1.0f, tempCB_Top);
	UI->addSlider(window.arbPosGLcoords(window.width - 180, 110), window.dimensionGLcoords(120, 35), &diffuse, 0.0f, 1.0f, tempCB_Bot);

	times.updateTime();

	//MISC
	////////////////////////////////////////
	crosshair.add(0.0f, 0.05f, 0.0f);
	crosshair.add(0.0f, -0.05f, 0.0f);
	float AR = (float)window.width / window.height;
	crosshair.add(0.05f / AR, 0.0f, 0.0f);
	crosshair.add(-0.05f / AR, 0.0f, 0.0f);
	crosshair.genVertBuffer();

	textBox = new UITextBox(UITextBox::HUD);
	textBox->setFont("Textures/Text/tp_Sans.txt");
	textBox->setPosition(-0.9f, 0.9f, 0.0f);
	textBox->addTextPixelSized("CUBESPRAWL!",32,window.width,window.height);




	tower.loadMesh("Assets/tower.mesh");
	tower.loadTexture("Textures/tower.bmp");

	int max = world.getHeight(24, 24);
	for (int y = 0; y < max; y++){
		if ((*(world.getCubePosition(24, y, 24))) == nullptr){
			tower.mesh->translate(24, y, 24);
			y += 32;
		}
	}

	for (int i = 0; i < tower.mesh->verticies.size(); i++){
		//testNorms.add(tower.mesh->verticies[i]);
		testNorms.add(tower.mesh->verticies[i] + (tower.mesh->normals[i]*0.01f));
	}
	testNorms.genVertBuffer();

	WorldObject::diffuseTexShader = &pvTexture;
	WorldObject::litTexShader = &pvLitTex;


}


void exit(){
	//TODO: Memory cleanup
	//Framebuffer
	//Images

	delete &world;
	glDeleteShader(pvArrTexture.theProgram);
	glDeleteShader(pvLitTex.theProgram);
	glDeleteShader(flatCol.theProgram);
	glDeleteShader(pvTexture.theProgram);
	//glDeleteShader(pvColorFBO.theProgram);

	delete UI;
}


int main(int argc, char **argv){

	window.width = 1280; window.height = 720;


	if (!glfwInit())
		exit();//EXIT_FAILURE

	myMonitor = glfwGetPrimaryMonitor();
	glfwWindowHint(GLFW_SAMPLES, 4);//Multisampling hint
	GLFWwindow* windowGLFW = glfwCreateWindow(window.width, window.height, "Sprawler", NULL, NULL);
	//GLFWwindow* windowGLFW = glfwCreateWindow(1600, 900, "FRUSTRUM", myMonitor, NULL);//fullscreen mode, change view below
	glfwMakeContextCurrent(windowGLFW);

	if (!windowGLFW){
		exit();
	}

	GLenum err = glewInit();
	if (GLEW_OK != err){
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// REGISTER CALLBACKS //////////////
	////////////////////////////////////////////////////////////////
	glfwSetKeyCallback(windowGLFW, key_callback);
	glfwSetWindowSizeCallback(windowGLFW, window_size_callback);
	glfwSetScrollCallback(windowGLFW, scroll_callback);
	glfwSetMouseButtonCallback(windowGLFW, mouse_button_callback);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// IMPORTANT SETS //////////////////
	////////////////////////////////////////////////////////////////
	window.primaryWindow = windowGLFW; // is this best way to use the pointer?
	glViewport(0, 0, window.width, window.height);

	glfwSetInputMode(windowGLFW, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPos(window.primaryWindow, window.width * 0.50, window.height * 0.50);
	window.mouseX = window.width * 0.50; window.mouseY = window.height * 0.50;


	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);//hinted level 4 above

	glfwSwapInterval(1);//




	initialize();
	while (!glfwWindowShouldClose(window.primaryWindow))
	{

		display();


	}

	exit();


}