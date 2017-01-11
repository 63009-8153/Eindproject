#include "TextMeshCreator.h"

TextMeshCreator::TextMeshCreator()
{
}

TextMeshCreator::~TextMeshCreator()
{
}

List<Line> TextMeshCreator::createStructure(GUIText text) {
	char[] chars = text.getTextString().toCharArray();
	List<Line> lines = new ArrayList<Line>();
	Line currentLine = new Line(metaData.getSpaceWidth(), text.getFontSize(), text.getMaxLineSize());
	Word currentWord = new Word(text.getFontSize());
	for (char c : chars) {
		int ascii = (int)c;
		if (ascii == SPACE_ASCII) {
			boolean added = currentLine.attemptToAddWord(currentWord);
			if (!added) {
				lines.add(currentLine);
				currentLine = new Line(metaData.getSpaceWidth(), text.getFontSize(), text.getMaxLineSize());
				currentLine.attemptToAddWord(currentWord);
			}
			currentWord = new Word(text.getFontSize());
			continue;
		}
		Character character = metaData.getCharacter(ascii);
		currentWord.addCharacter(character);
	}
	completeStructure(lines, currentLine, currentWord, text);
	return lines;
}

void TextMeshCreator::completeStructure(List<Line> lines, Line currentLine, Word currentWord, GUIText text) {
	bool added = currentLine.attemptToAddWord(currentWord);
	if (!added) {
		lines.add(currentLine);
		currentLine = new Line(metaData.getSpaceWidth(), text.getFontSize(), text.getMaxLineSize());
		currentLine.attemptToAddWord(currentWord);
	}
	lines.add(currentLine);
}

TextMeshData TextMeshCreator::createQuadVertices(GUIText text, List<Line> lines) {
	text.setNumberOfLines(lines.size());
	double curserX = 0f;
	double curserY = 0f;
	List<Float> vertices = new ArrayList<Float>();
	List<Float> textureCoords = new ArrayList<Float>();
	for (Line line : lines) {
		if (text.isCentered()) {
			curserX = (line.getMaxLength() - line.getLineLength()) / 2;
		}
		for (Word word : line.getWords()) {
			for (Character letter : word.getCharacters()) {
				addVerticesForCharacter(curserX, curserY, letter, text.getFontSize(), vertices);
				addTexCoords(textureCoords, letter.getxTextureCoord(), letter.getyTextureCoord(),
					letter.getXMaxTextureCoord(), letter.getYMaxTextureCoord());
				curserX += letter.getxAdvance() * text.getFontSize();
			}
			curserX += metaData.getSpaceWidth() * text.getFontSize();
		}
		curserX = 0;
		curserY += LINE_HEIGHT * text.getFontSize();
	}
	return new TextMeshData(listToArray(vertices), listToArray(textureCoords));
}

void TextMeshCreator::addVerticesForCharacter(double curserX, double curserY, Character character, double fontSize,
	List<Float> vertices) {
	double x = curserX + (character.getxOffset() * fontSize);
	double y = curserY + (character.getyOffset() * fontSize);
	double maxX = x + (character.getSizeX() * fontSize);
	double maxY = y + (character.getSizeY() * fontSize);
	double properX = (2 * x) - 1;
	double properY = (-2 * y) + 1;
	double properMaxX = (2 * maxX) - 1;
	double properMaxY = (-2 * maxY) + 1;
	addVertices(vertices, properX, properY, properMaxX, properMaxY);
}

void TextMeshCreator::addVertices(List<Float> vertices, double x, double y, double maxX, double maxY) {
	vertices.add((float)x);
	vertices.add((float)y);
	vertices.add((float)x);
	vertices.add((float)maxY);
	vertices.add((float)maxX);
	vertices.add((float)maxY);
	vertices.add((float)maxX);
	vertices.add((float)maxY);
	vertices.add((float)maxX);
	vertices.add((float)y);
	vertices.add((float)x);
	vertices.add((float)y);
}

void TextMeshCreator::addTexCoords(List<Float> texCoords, double x, double y, double maxX, double maxY) {
	texCoords.add((float)x);
	texCoords.add((float)y);
	texCoords.add((float)x);
	texCoords.add((float)maxY);
	texCoords.add((float)maxX);
	texCoords.add((float)maxY);
	texCoords.add((float)maxX);
	texCoords.add((float)maxY);
	texCoords.add((float)maxX);
	texCoords.add((float)y);
	texCoords.add((float)x);
	texCoords.add((float)y);
}


float[] TextMeshCreator::listToArray(List<Float> listOfFloats) {
	float[] array = new float[listOfFloats.size()];
	for (int i = 0; i < array.length; i++) {
		array[i] = listOfFloats.get(i);
	}
	return array;
}

TextMeshCreator::TextMeshCreator(File metaFile) {
	metaData = new MetaFile(metaFile);
}

TextMeshData TextMeshCreator::createTextMesh(GUIText text) {
	List<Line> lines = createStructure(text);
	TextMeshData data = createQuadVertices(text, lines);
	return data;
}
