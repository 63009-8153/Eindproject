#include "Character.h"

Character::Character()
{
}
Character::~Character()
{
}

Character::Character(int id, double xTextureCoord, double yTextureCoord, double xTexSize, double yTexSize, double xOffset, double yOffset, double sizeX, double sizeY, double xAdvance)
{
	id = id;
	xTextureCoord = xTextureCoord;
	yTextureCoord = yTextureCoord;
	xOffset = xOffset;
	yOffset = yOffset;
	sizeX = sizeX;
	sizeY = sizeY;
	xMaxTextureCoord = xTexSize + xTextureCoord;
	yMaxTextureCoord = yTexSize + yTextureCoord;
	xAdvance = xAdvance;
}

int Character::getId() {
	return id;
}

double Character::getxTextureCoord() {
	return xTextureCoord;
}

double Character::getyTextureCoord() {
	return yTextureCoord;
}

double Character::getXMaxTextureCoord() {
	return xMaxTextureCoord;
}

double Character::getYMaxTextureCoord() {
	return yMaxTextureCoord;
}

double Character::getxOffset() {
	return xOffset;
}

double Character::getyOffset() {
	return yOffset;
}

double Character::getSizeX() {
	return sizeX;
}

double Character::getSizeY() {
	return sizeY;
}

double Character::getxAdvance() {
	return xAdvance;
}
