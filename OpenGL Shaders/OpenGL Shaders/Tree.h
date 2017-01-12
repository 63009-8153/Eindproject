#pragma once

#include "Header.h"
#include "Model.h"

class Tree
{
public:
	Tree();
	~Tree();

	void setTreeModel(gameobject * model);
	void setTreeTexture(GLuint textID);

	void addTreePos(float x, float y, float z);
	void addTreePos(glm::vec3 pos);

	void initialseNewTrees();

	unsigned int getTreeCount();
	gameobject* getTreeAt(int iteration);

private:
	std::vector<Model> trees;
	gameobject treeModel;
	GLuint texture;
};

