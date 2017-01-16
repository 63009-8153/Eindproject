#pragma once

#include "../../Header.h"

class TextMeshData
{
	public:
		TextMeshData();
		~TextMeshData();

		TextMeshData(float *_vertexPositions, int _vertexAmount, float *_textureCoords, int _textureAmount);
	
		float* getVertexPositions();

		float* getTextureCoords();

		int getVertexCount();
		int getTextureCount();

	private:

		float* vertexPositions;
		int vertexCount;

		float* textureCoords;
		int textureCount;

};

