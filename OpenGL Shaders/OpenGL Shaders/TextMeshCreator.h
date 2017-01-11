#pragma once

#include "MetaFile.h"
#include "Word.h"
#include "Line.h"
#include "Character.h"

#define LINE_HEIGHT = 0.03f;
#define SPACE_ASCII = 32;

class TextMeshCreator
{
	public:
		TextMeshCreator();
		~TextMeshCreator();

		TextMeshCreator(File metaFile);

		TextMeshData createTextMesh(GUIText text);

	private:
		MetaFile metaData;

		List<Line> createStructure(GUIText text);

		void completeStructure(List<Line> lines, Line currentLine, Word currentWord, GUIText text);

		TextMeshData createQuadVertices(GUIText text, List<Line> lines);

		void addVerticesForCharacter(double curserX, double curserY, Character character, double fontSize,
			List<float> vertices);

		static void addVertices(List<float> vertices, double x, double y, double maxX, double maxY);

		static void addTexCoords(List<float> texCoords, double x, double y, double maxX, double maxY);


		static float[] listToArray(List<float> listOfFloats);
};

