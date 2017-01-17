#pragma once

class MetaFile;
#include "MetaFile.h"

class Word;
#include "Word.h"

class Line;
#include "Line.h"

class GUIText;
#include "GUIText.h"

class Character;
#include "Character.h"

class TextMeshData;
#include "TextMeshData.h"

#define LINE_HEIGHT 0.03f
#define SPACE_ASCII 32

class TextMeshCreator
{
	public:
		TextMeshCreator();
		~TextMeshCreator();

		TextMeshCreator(char * filepath);

		TextMeshData createTextMesh(GUIText &text);

	private:
		MetaFile metaData;

		std::vector<Line> createStructure(GUIText &text);

		void completeStructure(std::vector<Line> &lines, Line &currentLine, Word &currentWord, GUIText &text);

		TextMeshData createQuadVertices(GUIText &text, std::vector<Line> &lines);

		void addVerticesForCharacter(double curserX, double curserY, Character &character, double fontSize, std::vector<float> &vertices);

		static void addVertices(std::vector<float> &vertices, double x, double y, double maxX, double maxY);

		static void addTexCoords(std::vector<float> &texCoords, double x, double y, double maxX, double maxY);

		// This does not keep the pointer valid, you should copy its contents after use!
		static float* listToArray(std::vector<float> &listOfFloats);
};

