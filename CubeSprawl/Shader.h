#pragma once
#include <GL/glew.h>

#include <matrix4.h>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
/*
HOW TO USE THIS:

TODO


*/

//model matrix is translations, rotations, and scaling
//view matrix translates to camera/eye viewing pos


class Shader
{

public:
	Shader();
	~Shader();

	GLuint theProgram; 
	bool initialized = false;//for static shaders

	enum SAMPLER_SETTING{NONE, ANISTROPIC, TRILIN, BILIN, NEAR, NEAR_MIP};
	GLuint samplers[3]; GLuint samplerIndexes[3];// for texture programs

	GLuint uniform1; //generic use uniform set it on shader setup so as to not worry about the string name anymore than necc
	GLuint uniform2;
	GLuint uniform3;

	GLuint pvmUniformLoc;//could make this more dynamic since not everything uses it
	GLuint mUniformLoc;//modelmat only used for (some) lighting 

	static GLuint pv_UBO_index;//UBO unique index thing
	static GLuint pv_UBO;//location

	operator GLuint const(){
		return theProgram;
	}


	string readFile(const char * fileName){
		std::ifstream read;
		
		std::string holder = "";
		std::string str;

		read.open(fileName);

		if (read.is_open()){
			std::getline(read, str);
			holder += str;
			holder.append("\n");//important
		
		while (!read.eof()){
			std::getline(read, str);
			//cout << str.c_str() << endl;
			holder += str;
			}
		}
		return holder;
	}

	void InitializeProgram(const char* vertFile, const char* fragFile){
		std::vector<GLuint> shaderList;

		shaderList.push_back(CreateShader(GL_VERTEX_SHADER, readFile(vertFile)));
		shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, readFile(fragFile)));

		theProgram = glCreateProgram();
		for (GLuint shader : shaderList){
			glAttachShader(theProgram, shader);
		}
		//////////////////////////////////////////////////////
		glLinkProgram(theProgram);
		//////////////////////////////////////////////////////
		for (GLuint shader : shaderList){
			glDeleteShader(shader);
		}


	
}//program from file

	//Should make private or protected
	static GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
	{
		GLuint shader = glCreateShader(eShaderType);
		const char *strFileData = strShaderFile.c_str();
		glShaderSource(shader, 1, &strFileData, NULL);

		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

			const char *strShaderType = NULL;
			switch (eShaderType)
			{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			}

			fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
			delete[] strInfoLog;
		}

		return shader;
	}

	//create the global UBO with an existing program// Takes in a shader that wants to use it
	//   |      16      ||      16      ||      16      || 4 |
	//          p               v               pv         L
	//
	static void setGlobalPV_UBO(Shader * existingShader){
		pv_UBO_index = glGetUniformBlockIndex(existingShader->theProgram, "GlobalPV");

		glGenBuffers(1, &pv_UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, pv_UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 52, NULL, GL_STREAM_DRAW);
			//1-16:Persp. mat//16-32 view mat.// 32-48pers*view mat//48-51 sun direction 52 sun intensity
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUniformBlockBinding(existingShader->theProgram, pv_UBO_index, 1);//the association, 1 is a binding point
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, pv_UBO, 0, sizeof(float) * 52);//association pt 2, they look like they combine through index 1 now
	}
	//link the UBO to a shader that wants to use it // Use above for the first one
	void linkUBO(){
		glUniformBlockBinding(this->theProgram, Shader::pv_UBO_index, 1);
	}

	static void updateUPO_p(const matrix4* const pMat){
		matrix4 transp = pMat->transpose();
		glBindBuffer(GL_UNIFORM_BUFFER, pv_UBO);
		glBufferSubData(GL_UNIFORM_BUFFER,          0,         sizeof(float) * 16, &transp[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	static void updateUPO_v(const matrix4* const vMat){
		matrix4 transp = vMat->transpose();
		glBindBuffer(GL_UNIFORM_BUFFER, pv_UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, &transp[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	static void updateUPO_pv(const matrix4* const pvMat){
		matrix4 transp = pvMat->transpose();
		glBindBuffer(GL_UNIFORM_BUFFER, pv_UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 32, sizeof(float) * 16, &transp[0]); //&pvMat->matrix[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	static void updateUPO_light(const vector4 light){
		glBindBuffer(GL_UNIFORM_BUFFER, pv_UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 48, sizeof(float) * 4, &light.coords[0]); //&pvMat->matrix[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}


	void setUniform(const char* uniformID, int uniform){
		glUseProgram(theProgram);

		switch (uniform){
		case 1: {
			uniform1 = glGetUniformLocation(theProgram, uniformID);
			break;
		}
		case 2: {
			uniform2 = glGetUniformLocation(theProgram, uniformID);
			break;
		}
		case 3: {
			uniform3 = glGetUniformLocation(theProgram, uniformID);
			break;
		}
		default:
			break;
		}
		
		glUseProgram(0);
	}

	//level: Use 11 for bilin or trilin etc// level = number mips expected or xAniso
	//Sampler index starts with 0
	void setSampler(const char* fragShaderName, int samplerIndex,  SAMPLER_SETTING setting, int level, GLuint mode = GL_REPEAT){
		//TODO use a pointer to work on multiple samplers

		glUseProgram(theProgram);
		samplerIndexes[samplerIndex] = glGetUniformLocation(theProgram, fragShaderName);
		glGenSamplers(1, &samplers[samplerIndex]);
		glUniform1i(samplerIndexes[samplerIndex], samplerIndex);// same number for glActiveTexture( GL_TEXTURE0 ), GL_TEXTURE2 etc

		//glBindSampler(samplerIndex, samplers[samplerIndex]);
		switch (setting)
		{
		case Shader::ANISTROPIC:{
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glSamplerParameterf(samplerIndexes[samplerIndex], GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)level);
		}
			break;
		case Shader::TRILIN:{
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_S, mode);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_T, mode);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MAX_LEVEL, level);
		}
			break;
		case Shader::BILIN:{

		}
			break;
		case Shader::NEAR:{
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_S, mode);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_T, mode);
			//glSamplerParameteri(sampler1, GL_TEXTURE_MAX_LEVEL, level);
		}
			  break; 
		case Shader::NEAR_MIP:{
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_S, mode);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_WRAP_T, mode);
			glSamplerParameteri(samplerIndexes[samplerIndex], GL_TEXTURE_MAX_LEVEL, level);
		}
			break;
		case NONE:{
		}
			break;
		default:
			break;
		}


	glUseProgram(0);
	}


};//class


