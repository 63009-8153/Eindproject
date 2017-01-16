#include "TextMeshCreator.h"

TextMeshCreator::TextMeshCreator()
{
}

TextMeshCreator::~TextMeshCreator()
{
}

TextMeshCreator::TextMeshCreator(char * filepath) {
	metaData = MetaFile(filepath);
}

TextMeshData TextMeshCreator::createTextMesh(GUIText &text) {
	std::vector<Line> lines = createStructure(&text);

	TextMeshData data = createQuadVertices(text, lines);

	return data;
}

std::vector<Line> TextMeshCreator::createStructure(GUIText *text) {
	std::string charString = text->getTextString();

	std::vector<Line> lines;

	Line currentLine = Line(metaData.getSpaceWidth(), text->getFontSize(), text->getMaxLineSize());
	Word currentWord = Word(text->getFontSize());

	// Loop through all characters
	for (int i = 0; i < charString.size(); i++) {
		char c = charString[i];

		int ascii = (int)c;
		if (ascii == SPACE_ASCII) {
			bool added = currentLine.attemptToAddWord(currentWord);
			if (!added) {
				lines.push_back(currentLine);
				currentLine = Line(metaData.getSpaceWidth(), text->getFontSize(), text->getMaxLineSize());
				currentLine.attemptToAddWord(currentWord);
			}
			currentWord = Word(text->getFontSize());

			continue;
		}

		Character character = metaData.getCharacter(ascii);
		currentWord.addCharacter(character);
	}

	completeStructure(lines, currentLine, &currentWord, text);

	return lines;
}

void TextMeshCreator::completeStructure(std::vector<Line> &lines, Line &currentLine, Word *currentWord, GUIText *text) {
	bool added = currentLine.attemptToAddWord(*currentWord);

	if (!added) {
		lines.push_back(currentLine);
		currentLine = Line(metaData.getSpaceWidth(), text->getFontSize(), text->getMaxLineSize());
		currentLine.attemptToAddWord(*currentWord);
	}

	lines.push_back(currentLine);
}

TextMeshData TextMeshCreator::createQuadVertices(GUIText &text, std::vector<Line> &lines) {
	text.setNumberOfLines(lines.size());
	double curserX = 0.0f;
	double curserY = 0.0f;

	std::vector<float> vertices, textureCoords;

	for (Line line : lines) {
		if (text.isCentered()) {
			curserX = (line.getMaxLength() - line.getLineLength()) / 2;
		}

		for (Word word : line.getWords()) {
			for (Character letter : word.getCharacters()) {
				addVerticesForCharacter(curserX, curserY, &letter, text.getFontSize(), vertices);
				addTexCoords(textureCoords, letter.getxTextureCoord(), letter.getyTextureCoord(),
					letter.getXMaxTextureCoord(), letter.getYMaxTextureCoord());
				curserX += letter.getxAdvance() * text.getFontSize();
			}
			curserX += metaData.getSpaceWidth() * text.getFontSize();
		}

		curserX = 0;
		curserY += LINE_HEIGHT * text.getFontSize();
	}

	return TextMeshData(listToArray(vertices), vertices.size(), listToArray(textureCoords), textureCoords.size());
}

void TextMeshCreator::addVerticesForCharacter(double curserX, double curserY, Character *character, double fontSize, std::vector<float> &vertices) {
	
	double x = curserX + (character->getxOffset() * fontSize);
	double y = curserY + (character->getyOffset() * fontSize);
	double maxX = x + (character->getSizeX() * fontSize);
	double maxY = y + (character->getSizeY() * fontSize);
	double properX = (2 * x) - 1;
	double properY = (-2 * y) + 1;
	double properMaxX = (2 * maxX) - 1;
	double properMaxY = (-2 * maxY) + 1;
	
	addVertices(vertices, properX, properY, properMaxX, properMaxY);
}

void TextMeshCreator::addVertices(std::vector<float> &vertices, double x, double y, double maxX, double maxY) {
	vertices.push_back((float)x);
	vertices.push_back((float)y);
	vertices.push_back((float)x);
	vertices.push_back((float)maxY);
	vertices.push_back((float)maxX);
	vertices.push_back((float)maxY);
	vertices.push_back((float)maxX);
	vertices.push_back((float)maxY);
	vertices.push_back((float)maxX);
	vertices.push_back((float)y);
	vertices.push_back((float)x);
	vertices.push_back((float)y);
}
void TextMeshCreator::addTexCoords(std::vector<float> &texCoords, double x, double y, double maxX, double maxY) {
	texCoords.push_back((float)x);
	texCoords.push_back((float)y);
	texCoords.push_back((float)x);
	texCoords.push_back((float)maxY);
	texCoords.push_back((float)maxX);
	texCoords.push_back((float)maxY);
	texCoords.push_back((float)maxX);
	texCoords.push_back((float)maxY);
	texCoords.push_back((float)maxX);
	texCoords.push_back((float)y);
	texCoords.push_back((float)x);
	texCoords.push_back((float)y);
}


float* TextMeshCreator::listToArray(std::vector<float> &listOfFloats) {
	float *arr = new float[listOfFloats.size()];
	for (int i = 0; i < listOfFloats.size(); i++) {
		arr[i] = listOfFloats[i];
	}

	return arr;
}
