#include "../header.h"

#include "renderer.h"

Renderer::Renderer()
{
}
Renderer::Renderer(ShaderProgram *shad)
{
	shader = shad;
}

void Renderer::render(std::map<GLuint, std::vector<gameobject>> *gameobjects)
{
	std::map<GLuint, std::vector<gameobject>>::iterator it;

	for (it = gameobjects->begin(); it != gameobjects->end(); it++) {
		//Prepare variables for all model
		prepareMassModel(&it->second[0]);

		//Loop through all model by texture
		for (unsigned int i = 0; i < it->second.size(); i++) {
			//Prepare each models own variables
			prepareModelInstance(&it->second[i]);

			//If the model is supposed to be using both sides, disable culling
			if (!it->second[i].cullFaces) MasterRenderer::DisableCulling();

			//Draw each model
			glDrawElements(GL_TRIANGLES, it->second[i].getVertexCount(), GL_UNSIGNED_INT, 0);
			//Always enable culling again
			if (!it->second[i].cullFaces) MasterRenderer::EnableCulling();
		}

		//Unbind the mass used model
		unbindModel();
	}
}

void Renderer::prepareMassModel(gameobject *object)
{
	//Use the vao of this object
	glBindVertexArray(object->getVaoID());

	//Use the positions location
	glEnableVertexAttribArray(0); //Enable using VAO position 0 (positions)
	glEnableVertexAttribArray(1); //Enable using VAO position 1 (uv coords)
	glEnableVertexAttribArray(2); //Enable using VAO position 2 (normal coords)
	glEnableVertexAttribArray(3); //Enable using VAO position 3 (tangents)

	int textureAmount = object->getTextureAmount();

	shader->loadNumberOfRows(object->numberOfRows);

	//Use texture
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
void Renderer::unbindModel()
{
	glDisableVertexAttribArray(0); //Stop using VAO position 0 (positions)
	glDisableVertexAttribArray(1); //Stop using VAO position 1 (uv coords)
	glDisableVertexAttribArray(2); //Stop using VAO position 2 (normal coords)
	glDisableVertexAttribArray(3); //Stop using VAO position 3 (tangents)

	//Unbind vao
	glBindVertexArray(0);
}

void Renderer::prepareModelInstance(gameobject *object)
{
	//Load the hasTransparency in the shader
	shader->loadFakeLighting(object->useFakeLighting);
	shader->loadAmbientLight(object->getAmbientLight());

	//If model has a normal map load this to texture 5
	if (object->hasNormalMap) {
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, object->getNormalMapID());
	}
	//If model has a shadow map load this to texture 6
	if (object->hasShadowMap) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, object->getShadowMapID());
	}
	//If model has a specular map load this to texture 7
	shader->loadUseSpecularMap(object->hasSpecularMap);
	if (object->hasSpecularMap) {
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, object->getSpecularMapID());
	}
	//If model has a enviroment cubemap load this to texture 8
	if (object->hasReflectionCubeMap) {
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_CUBE_MAP, object->getEnviromentCubeMapID());
	}
	shader->loadUseReflectionCubeMap(object->hasReflectionCubeMap);
	shader->loadUseRefractionCubeMap(object->hasRefractionCubeMap);

	shader->loadReflectionRefractionRatio(object->reflectionRefractionRatio);
	shader->loadReflectionColourRatio(object->reflectionRatio);

	//Create a transformation matrix from the object data
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(object->getPosition(), object->getRotation(), object->getScale());
	//Load the transformation matrix into the shader for use
	shader->loadTransformationMatrix(transformationMatrix);

	shader->loadOffset(glm::vec2(object->getTextureXOffset(), object->getTextureYOffset()));

	//Load the object shinyness and reflectivityness
	shader->loadShineVariables(object->getShineDamper(), object->getReflectivity());
}
