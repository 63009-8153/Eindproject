#pragma once
class TextMeshData
{
public:
	TextMeshData();
	~TextMeshData();

	TextMeshData(float[] vertexPositions, float[] textureCoords);
	
	float[] getVertexPositions();

	float[] getTextureCoords();

	int getVertexCount();

private:

	float[] vertexPositions;
	float[] textureCoords;

};

