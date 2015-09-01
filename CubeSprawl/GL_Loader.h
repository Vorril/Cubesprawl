#pragma once


#include "GL/glew.h"//using the d library nd dll
#include "GL/SOIL.h"
#include "vec4VBO.h"
#include <string>

namespace GL_Loader{
	using namespace std;
	//relies on a naming convention...// Mips: 1 = just base image// 2= base+1 mip... etc
	static void loadArrayTexture(GLuint& texture, int wid, int hi, int mips, vector<string>& files, bool automip = false){

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

		if (automip) mips = 1;
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mips, GL_RGB8, wid, hi, files.size());


		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

		for (int i = 0; i < files.size(); i++){
			vector<unsigned char*> buffers;

			int w, h, c;
			for (int f = 0; f < mips; f++){
				string fileLoc = files[i];
				if (!automip){//naming convention for handmade mips
					fileLoc.push_back('m');
					fileLoc.push_back((char)(f + 0x0030));
				}
				fileLoc.append(".bmp");

				

				buffers.push_back(SOIL_load_image(fileLoc.c_str(), &w, &h, &c, SOIL_LOAD_AUTO));
				//if w h c are wrong.... could check for this
				if (buffers.back() == nullptr){// || wid != w || hi != h
					cout << "MISSING TEXTURE: " << fileLoc << endl;//TEMP
					return;
				}
			}

			for (int t = 0; t < mips; t++){
				int dim = (int)(wid / (std::pow(2, t)));
				//glTexImage3D(GL_TEXTURE_2D_ARRAY, t, GL_RGB8, dim,dim, files.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, buffers[t]);
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, t, 0, 0, i, dim, dim, 1, GL_RGB, GL_UNSIGNED_BYTE, buffers[t]);
				free(buffers[t]);
			}

		}



		if (automip)
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);


	}

	//No  manual mip uploading
	//Naming convention (example):
	// Textures/tex1.bmp
	// Textures/tex1NM.bmp
	//Push "Textures/tex1" into files
	static void loadArrayTextureNormalMapped(GLuint& texture, GLuint& normMaps, int wid, int hi, vector<string>& files, int maxNumMips = 1024 ){
		int mipLevel = 1;
		int dimCopy = wid < hi ? wid : hi;

		while (dimCopy > 2){
			mipLevel += 1;
			dimCopy /= 2;
		}//strictly speaking the 1 pixel mip is a thing I think, this wont make it

		mipLevel = (maxNumMips < mipLevel) ? maxNumMips : mipLevel;//take the smaller value
		//if (maxNumMips < mipLevel) mipLevel = maxNumMips;

		////////////////////////////////
		// Generation ////////////////
		///////////////////////////////////////

		/////////// Texture //////////////////////////////////////////////////////
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevel, GL_RGBA8, wid, hi, files.size());//method 1 (1/2) no mips
		//glTexImage3D(GL_TEXTURE_2D_ARRAY, 9, GL_RGB8, wid, hi, files.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);//nothing

		for (int i = 0; i < files.size(); i++){
			unsigned char* buffer;

			int w, h, c;
		
			string fileLoc = files[i];
			fileLoc.append(".png");

			buffer = (SOIL_load_image(fileLoc.c_str(), &w, &h, &c, SOIL_LOAD_RGBA));
			if (buffer == nullptr){// || wid != w || hi != h
				cout << "MISSING TEXTURE: " << fileLoc << endl;//TEMP
					return;
			}
		
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, wid, hi, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);//method 1 (2/2) no mips
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, wid, hi, files.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
			
			free(buffer);
		}
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
			

		/////// Normal Map ////////////////////////////////////////////////////////////////////////////
		glGenTextures(1, &normMaps);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_ARRAY, normMaps);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevel, GL_RGB8, wid, hi, files.size());

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		for (int i = 0; i < files.size(); i++){
			vector<unsigned char*> buffers;

			int w, h, c;

			string fileLoc = files[i];
			fileLoc.append("NM.bmp");

			buffers.push_back(SOIL_load_image(fileLoc.c_str(), &w, &h, &c, SOIL_LOAD_AUTO));
			//if w h c are wrong.... could check for this
			if (buffers.back() == nullptr){// || wid != w || hi != h
				cout << "MISSING TEXTURE: " << fileLoc << endl;//TEMP
				return;
			}


			//glTexImage3D(GL_TEXTURE_2D_ARRAY, t, GL_RGB8, dim,dim, files.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, buffers[t]);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, wid, hi, 1, GL_RGB, GL_UNSIGNED_BYTE, buffers[0]);
			free(buffers[0]);


			
		}
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		
	}

	static void loadTexture(GLuint &texture, bool flagMips, bool compress, const char* File){
		unsigned int flags = SOIL_FLAG_INVERT_Y;
		if(compress) flags |= SOIL_FLAG_COMPRESS_TO_DXT;
		if (flagMips) flags |= SOIL_FLAG_MIPMAPS;

		texture = SOIL_load_OGL_texture(
			File,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			flags
			);
	}

	static bool loadObj(const char* filePath, vec4VBO* loadToMe){
		FILE * file = fopen(filePath, "r");
		if (file == NULL){
			//printf("Impossible to open the file !\n");
			return false;
		}
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< vector4 > temp_vertices;
		std::vector< vector2 > temp_uvs;
		std::vector< vector3 > temp_normals;

		while (1){

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			if (strcmp(lineHeader, "v") == 0){
				vector4 vertex;
				fscanf(file, "%f %f %f\n", &vertex.coords[0], &vertex.coords[1], &vertex.coords[2]);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0){
				vector2 uv;
				fscanf(file, "%f %f\n", &uv.coords[0], &uv.coords[1]);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0){
				vector3 normal;
				fscanf(file, "%f %f %f\n", &normal.coords[0], &normal.coords[1], &normal.coords[2]);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0){
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9){
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return false;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}//while
		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++){
			unsigned int vertexIndex = vertexIndices[i];
			vector4 vertex = temp_vertices[vertexIndex - 1];
			loadToMe->verticies.push_back(vertex);

			unsigned int uvIndex = uvIndices[i];
			vector2 uvVert = temp_uvs[uvIndex - 1];
			loadToMe->texCoords.push_back(uvVert);

			unsigned int normIndex = normalIndices[i];
			vector3 normVert = temp_normals[normIndex - 1];
			loadToMe->addNorm(normVert);
		}

		return true;

	}//loadObj


}