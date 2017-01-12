#include "Character.h"

Character::Character()
{
}
Character::~Character()
{
}

Character::Character(int _id, double _xTextureCoord, double _yTextureCoord, double _xTexSize, double _yTexSize, double _xOffset, double _yOffset, double _sizeX, double _sizeY, double _xAdvance)
{
	id = _id;
	xTextureCoord = _xTextureCoord;
	yTextureCoord = _yTextureCoord;
	xOffset = _xOffset;
	yOffset = _yOffset;
	sizeX = _sizeX;
	sizeY = _sizeY;
	xMaxTextureCoord = _xTexSize + _xTextureCoord;
	yMaxTextureCoord = _yTexSize + _yTextureCoord;
	xAdvance = _xAdvance;
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
