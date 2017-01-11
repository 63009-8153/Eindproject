#pragma once
class TextMeshData
{
public:
	TextMeshData();
	~TextMeshData();

	float[] getVertexPositions();

	float[] getTextureCoords();

	int getVertexCount();

private:

	float[] vertexPositions;
	float[] textureCoords;

protected:
	TextMeshData(float[] vertexPositions, float[] textureCoords);

};

