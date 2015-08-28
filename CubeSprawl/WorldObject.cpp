#include "WorldObject.h"

Shader* WorldObject::diffuseTexShader = nullptr;
Shader* WorldObject::litTexShader = nullptr;
Shader* WorldObject::litTexPVMShader = nullptr;

matrix4 const* WorldObject::cameraPV = nullptr;


WorldObject::WorldObject()
{
}


WorldObject::~WorldObject()
{
	delete mesh;//TODO not this
}


void WorldObject::loadMesh(const char* File){
	mesh = new vec4VBO();
	GL_Loader::loadObj(File, mesh);
	mesh->genVertBuffer();
	mesh->genUVBuffer();
	mesh->genNormBuffer();
}

void WorldObject::loadTexture(const char* File, bool flagMips, bool compress){
	GL_Loader::loadTexture(texture, flagMips, compress, File);
}

//TODO write versions of these that dont unbind program
void WorldObject::draw()const{
	glUseProgram(*(WorldObject::diffuseTexShader));

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // uv tex coords

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, diffuseTexShader->samplers[0]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->UV_BUFF_ID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, mesh->verticies.size());

}

void WorldObject::drawLit()const{
	glUseProgram(*(WorldObject::litTexShader));

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // uv tex coords
	glEnableVertexAttribArray(2); // norms

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, litTexShader->samplers[0]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->UV_BUFF_ID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->NORM_BUFF_ID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, mesh->verticies.size());
}

void WorldObject::drawLitPVM()const{
	const matrix4 pvm = *cameraPV * m_Matrix;

	glUseProgram(*litTexPVMShader);
	glUniformMatrix4fv(litTexPVMShader->uniform1, 1, false, &((pvm.transpose()).matrix[0]));

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // uv tex coords
	glEnableVertexAttribArray(2); // norms

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, litTexPVMShader->samplers[0]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->VERT_BUFF_ID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->UV_BUFF_ID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->NORM_BUFF_ID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, mesh->verticies.size());
}