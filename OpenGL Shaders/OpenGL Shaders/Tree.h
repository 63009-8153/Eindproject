#pragma once

#include "Header.h"
#include "Model.h"

class Tree
{
public:
	Tree();
	~Tree();

	void setTreeModel(gameobject * model);

	void addTreePos(float x, float y, float z);
	void addTreePos(glm::vec3 pos);

	void initialseNewTrees();

	int getTreeCount();
	gameobject* getTreeAt(int iteration);

private:
	std::vector<Model> trees;
	gameobject treeModel;
};

