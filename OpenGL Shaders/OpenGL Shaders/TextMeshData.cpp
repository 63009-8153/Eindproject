#include "TextMeshData.h"

TextMeshData::TextMeshData()
{
}

TextMeshData::~TextMeshData()
{
}

TextMeshData::TextMeshData(float* _vertexPositions, int _vertexAmount, float *_textureCoords, int _textureAmount) {
	memcpy(vertexPositions, _vertexPositions, _vertexAmount);
	vertexCount = _vertexAmount;

	memcpy(textureCoords, _textureCoords, _textureAmount);
	textureCount = _textureAmount;
}

float* TextMeshData::getVertexPositions() {
	return vertexPositions;
}

float* TextMeshData::getTextureCoords() {
	return textureCoords;
}

int TextMeshData::getVertexCount() {
	return vertexCount / 2;
}