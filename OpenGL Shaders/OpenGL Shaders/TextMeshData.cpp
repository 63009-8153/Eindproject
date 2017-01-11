#include "TextMeshData.h"

TextMeshData::TextMeshData()
{
}

TextMeshData::~TextMeshData()
{
}

float[] TextMeshData::getVertexPositions() {
	return vertexPositions;
}

float[] TextMeshData::getTextureCoords() {
	return textureCoords;
}

int TextMeshData::getVertexCount() {
	return vertexPositions.length / 2;
}


TextMeshData::TextMeshData(float[] vertexPositions, float[] textureCoords) {
	vertexPositions = vertexPositions;
	textureCoords = textureCoords;
}