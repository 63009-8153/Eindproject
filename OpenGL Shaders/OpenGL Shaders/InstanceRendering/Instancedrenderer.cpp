#include "header.h"

#include "Instancedrenderer.h"

InstancedRenderer::InstancedRenderer()
{
}
InstancedRenderer::InstancedRenderer(InstancedShaderProgram *shad)
{
	shader = shad;
}

void InstancedRenderer::render(std::map<GLuint, std::vector<gameobject>> *gameobjects, int Instance_data_length, GLuint vao, GLuint vbo)
{
	std::map<GLuint, std::vector<gameobject>>::iterator it;

	prepare(vao);

	for (it = gameobjects->begin(); it != gameobjects->end(); it++) { //Each texture
		//Prepare variables for all model
		prepareModel(&it->second[0]);

		pointer = 0;

		float *vboData = new float[it->second.size() * Instance_data_length];

		//Loop through all model by texture
		for (unsigned int i = 0; i < it->second.size(); i++) { //Each gamemodel with texture
			//Load the hasTransparency in the shader
			shader->loadFakeLighting(it->second[i].useFakeLighting);

			//If the model is supposed to be using both sides, disable culling
			if (!it->second[i].cullFaces) InstanceMasterRenderer::DisableCulling();

			//Create a transformation matrix from the object data
			glm::mat4 transformationMatrix = Maths::createTransformationMatrix(it->second[i].getPosition(), it->second[i].getRotation(), it->second[i].getScale());
			
			
			/*vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;
			vboData[pointer++] = 1.0f;*/
			vboData[pointer++] = transformationMatrix[0][0];
			vboData[pointer++] = transformationMatrix[0][1];
			vboData[pointer++] = transformationMatrix[0][2];
			vboData[pointer++] = transformationMatrix[0][3];
			vboData[pointer++] = transformationMatrix[1][0];
			vboData[pointer++] = transformationMatrix[1][1];
			vboData[pointer++] = transformationMatrix[1][2];
			vboData[pointer++] = transformationMatrix[1][3];
			vboData[pointer++] = transformationMatrix[2][0];
			vboData[pointer++] = transformationMatrix[2][1];
			vboData[pointer++] = transformationMatrix[2][2];
			vboData[pointer++] = transformationMatrix[2][3];
			vboData[pointer++] = transformationMatrix[3][0];
			vboData[pointer++] = transformationMatrix[3][1];
			vboData[pointer++] = transformationMatrix[3][2];
			vboData[pointer++] = transformationMatrix[3][3];

			vboData[pointer++] = it->second[i].getShineDamper();
			vboData[pointer++] = it->second[i].getReflectivity();
			vboData[pointer++] = it->second[i].getAmbientLight();

			vboData[pointer++] = it->second[i].getTextureXOffset();
			vboData[pointer++] = it->second[i].getTextureYOffset();
		}

		shader->loadFakeLighting(it->second[0].useFakeLighting);
		if (!it->second[0].cullFaces) InstanceMasterRenderer::DisableCulling();

		Loader loader;
		loader.updateVbo(vbo, vboData, pointer);

		unbindModel();
		prepare(vao);

		glDrawElementsInstanced(GL_TRIANGLES, it->second[0].getVertexCount(), GL_UNSIGNED_INT, 0, it->second.size());
		
		//Always enable culling again
		InstanceMasterRenderer::EnableCulling();
		//Unbind the mass used model
		unbindModel();
	}
}

void InstancedRenderer::prepareModel(gameobject *object)
{
	//Get the amount of textures the object has for multitexturing
	int textureAmount = object->getTextureAmount();

	//If model has a normal map load this to texture 5
	if (object->hasNormalMap) {
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, object->getNormalMapID());
	}

	//Use texture(s)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->getTextureID(0));
	if (textureAmount >= 2) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, object->getTextureID(1));
	}
	if (textureAmount >= 3) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, object->getTextureID(2));
	}
	if (textureAmount >= 4) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, object->getTextureID(3));
	}
	if (textureAmount >= 5) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, object->getTextureID(4));
	}
	if (textureAmount > 6) {
		printf("Maximum supported textures are 5!");
	}
}
void InstancedRenderer::prepare(GLuint vao)
{
	//Use the vao of this object
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0); //Enable using VAO position 0 (positions)
	glEnableVertexAttribArray(1); //Enable using VAO position 1 (uv coords)
	glEnableVertexAttribArray(2); //Enable using VAO position 2 (normals)
	glEnableVertexAttribArray(3); //Enable using VAO position 3 (tangents)

	glEnableVertexAttribArray(4); //Enable using VAO position 4 (transformationMatrix Row 1)
	glEnableVertexAttribArray(5); //Enable using VAO position 5 (transformationMatrix Row 2)
	glEnableVertexAttribArray(6); //Enable using VAO position 6 (transformationMatrix Row 3)
	glEnableVertexAttribArray(7); //Enable using VAO position 7 (transformationMatrix Row 4)

	glEnableVertexAttribArray(8); //Enable using VAO position 8 (ShineReflectAmbient)
	glEnableVertexAttribArray(9); //Enable using VAO position 9 (offset)
}
void InstancedRenderer::unbindModel()
{
	glDisableVertexAttribArray(0); //Stop using VAO position 0 (positions)
	glDisableVertexAttribArray(1); //Stop using VAO position 1 (uv coords)
	glDisableVertexAttribArray(2); //Stop using VAO position 2 (normals)
	glDisableVertexAttribArray(3); //Stop using VAO position 3 (tangents)

	glDisableVertexAttribArray(4); //Enable using VAO position 4 (transformationMatrix Row 1)
	glDisableVertexAttribArray(5); //Enable using VAO position 5 (transformationMatrix Row 2)
	glDisableVertexAttribArray(6); //Enable using VAO position 6 (transformationMatrix Row 3)
	glDisableVertexAttribArray(7); //Enable using VAO position 7 (transformationMatrix Row 4)

	glDisableVertexAttribArray(8); //Enable using VAO position 8 (ShineReflectAmbient)
	glDisableVertexAttribArray(9); //Enable using VAO position 9 (offset)

	//Unbind vao
	glBindVertexArray(0);
}

void InstancedRenderer::printError(int t)
{
	printf("\n%d\n", t);
	int error = 0;
	for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
	{
		switch (err) {
		case GL_INVALID_ENUM:
			error = 1;
			break;
		case GL_INVALID_VALUE:
			error = 2;
			break;
		case GL_INVALID_OPERATION:
			error = 3;
			break;
		case GL_STACK_OVERFLOW:
			error = 4;
			break;
		case GL_STACK_UNDERFLOW:
			error = 5;
			break;
		case GL_OUT_OF_MEMORY:
			error = 6;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = 7;
			break;
		case GL_CONTEXT_LOST:
			error = 8;
			break;
		case GL_TABLE_TOO_LARGE:
			error = 9;
			break;
		}
		printf("Error: %d\n", error);
	}
}