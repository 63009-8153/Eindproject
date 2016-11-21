#include "../header.h"

#include "GuiRenderer.h"

GuiRenderer::GuiRenderer()
{
}

GuiRenderer::~GuiRenderer()
{
	shader.cleanUp();
}

void GuiRenderer::load(char vertexFile[], char fragmentFile[])
{
	float positions[8] = { -1, 1, -1, -1, 1, 1, 1, -1 };
	quad = loader.loadToVAO(positions, 8, 2);

	shader.load(vertexFile, fragmentFile);
}

void GuiRenderer::render(std::vector<texture2D> *textures)
{
	shader.start();
	glBindVertexArray(quad.getVaoID());
	glEnableVertexAttribArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	for (int i = 0; i < textures->size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures->at(i).textureid);

		glm::mat4 matrix = Maths::createTransformationMatrix(textures->at(i).position, textures->at(i).rotation, textures->at(i).scale);
		shader.loadTransformationMatrix(matrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, quad.getVertexCount());
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	shader.stop();
}