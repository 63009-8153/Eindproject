#include "Tree.h"



Tree::Tree()
{
}


Tree::~Tree()
{
}

void Tree::setTreeModel(gameobject * model)
{
	treeModel = gameobject(model);
	treeModel.init();
}

void Tree::setTreeTexture(GLuint textID)
{
	texture = textID;
}

void Tree::addTreePos(float x, float y, float z)
{
	Model newtree;
	newtree.setModel(&treeModel);
	newtree.Initialise(glm::vec3(x, y, z), glm::vec3(0.0f), glm::vec3(1.0f));
	newtree.getModel()->addTexture(texture);
	trees.push_back(newtree);
}

void Tree::addTreePos(glm::vec3 pos)
{
	Model newtree;
	newtree.setModel(&treeModel);
	newtree.Initialise(pos, glm::vec3(0.0f), glm::vec3(1.0f));
	trees.push_back(newtree);
}

void Tree::initialseNewTrees()
{
	// Set the random seed
	srand(time(NULL));

	// If there are no trees return
	if (trees.size() < 1) return;

	for (unsigned int i = 0; i < trees.size(); i++)
	{
		if (trees[i].getRotation() != glm::vec3(0.0f)) continue; // If the tree is already initialised, go to next tree

		glm::vec3 rot(0.0f);
		// Create a new random rotation which is up to 3 numbers precise and convert the degrees to radians
		rot.y = glm::radians((float)((rand() % 360000) / 100));

		// Set the new rotation
		trees[i].setRotation(rot);
	}
}

int Tree::getTreeCount()
{
	return (int)trees.size();
}

gameobject* Tree::getTreeAt(int iteration)
{
	return trees[iteration].getModel();
}
