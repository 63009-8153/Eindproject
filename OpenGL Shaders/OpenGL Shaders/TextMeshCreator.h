#pragma once

#define LINE_HEIGHT = 0.03f;
#define SPACE_ASCII = 32;

class TextMeshCreator
{
	public:
		TextMeshCreator();
		~TextMeshCreator();

	private:
		MetaFile metaData;

		List<Line> createStructure(GUIText text);

		void completeStructure(List<Line> lines, Line currentLine, Word currentWord, GUIText text);

		TextMeshData createQuadVertices(GUIText text, List<Line> lines);

		void addVerticesForCharacter(double curserX, double curserY, Character character, double fontSize,
			List<Float> vertices);

		static void addVertices(List<Float> vertices, double x, double y, double maxX, double maxY);

		static void addTexCoords(List<Float> texCoords, double x, double y, double maxX, double maxY);


		static float[] listToArray(List<Float> listOfFloats);

	protected:
		TextMeshCreator(File metaFile);

		TextMeshData createTextMesh(GUIText text);
};

