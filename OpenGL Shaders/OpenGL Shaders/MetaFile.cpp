#include "MetaFile.h"

// String split functions
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;

	while (std::getline(ss, item, delim)) {
		if(!item.empty()) elems.push_back(item);
	}
}
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

MetaFile::MetaFile()
{
}

MetaFile::~MetaFile()
{
}

/**
* Opens a font file in preparation for reading.
*
* @param file
*            - the font file.
*/
MetaFile::MetaFile(char * filepath) {
	aspectRatio = (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT;

	openFile(filepath);

	loadPaddingData();
	loadLineSizes();

	int imageWidth = getValueOfVariable("scaleW");

	loadCharacterData(imageWidth);

	close();
}

/**
* Read in the next line and store the variable values.
*
* @return {@code true} if the end of the file hasn't been reached.
*/
bool MetaFile::processNextLine() {
	values.clear();

	std::string line = std::string();

	char lineHeader[128]; //Lineheader buffer

	if (fgets(lineHeader, sizeof(lineHeader), file) == NULL) {
		return false;
	}

	std::vector<std::string> parts = split(line, SPLITTER);

	for (std::string part : parts) {
		std::vector<std::string> valuePairs = split(part, '=');

		if (values.size() == 2) {
			values.insert(std::pair<std::string, std::string>(valuePairs[0], valuePairs[1]));
		}
	}

	return true;
}

/**
* Gets the {@code int} value of the variable with a certain name on the
* current line.
*
* @param variable
*            - the name of the variable.
* @return The value of the variable.
*/
int MetaFile::getValueOfVariable(std::string variable) {
	return std::stoi(values.find(variable)->second);
}

/**
* Gets the array of ints associated with a variable on the current line.
*
* @param variable
*            - the name of the variable.
* @return The int array of values associated with the variable.
*/
int* MetaFile::getValuesOfVariable(std::string variable) {
	std::vector<std::string> numbers = split(values.find(variable)->second, NUMBER_SEPARATOR);

	int *actualValues = new int[numbers.size()];
	for (int i = 0; i < numbers.size(); i++) {
		actualValues[i] = std::stoi(numbers[i]);
	}

	return actualValues;
}

/**
* Closes the font file after finishing reading.
*/
void MetaFile::close() {
	fclose(file);
}

/**
* Opens the font file, ready for reading.
*
* @param file
*            - the font file.
*/
void MetaFile::openFile(char * filepath) {
	fopen_s(&file, filepath, "r");
	if (file == NULL) {
		printf("Impossible to open file: %s!\n", filepath);

		system("pause");
		exit(0);
	}
}

/**
* Loads the data about how much padding is used around each character in
* the texture atlas.
*/
void MetaFile::loadPaddingData() {
	processNextLine();

	padding = getValuesOfVariable("padding");
	paddingWidth = padding[PAD_LEFT] + padding[PAD_RIGHT];
	paddingHeight = padding[PAD_TOP] + padding[PAD_BOTTOM];
}

/**
* Loads information about the line height for this font in pixels, and uses
* this as a way to find the conversion rate between pixels in the texture
* atlas and screen-space.
*/
void MetaFile::loadLineSizes() {
	processNextLine();

	int lineHeightPixels = getValueOfVariable("lineHeight") - paddingHeight;
	verticalPerPixelSize = LINE_HEIGHT / (double)lineHeightPixels;
	horizontalPerPixelSize = verticalPerPixelSize / aspectRatio;
}

/**
* Loads in data about each character and stores the data in the
* {@link Character} class.
*
* @param imageWidth
*            - the width of the texture atlas in pixels.
*/
void MetaFile::loadCharacterData(int imageWidth) {
	processNextLine();
	processNextLine();

	while (processNextLine()) {
		Character c = loadCharacter(imageWidth);
		metaData.insert(std::pair<int, Character>(c.getId(), c));
	}
}

/**
* Loads all the data about one character in the texture atlas and converts
* it all from 'pixels' to 'screen-space' before storing. The effects of
* padding are also removed from the data.
*
* @param imageSize
*            - the size of the texture atlas in pixels.
* @return The data about the character.
*/
Character MetaFile::loadCharacter(int imageSize) {
	int id = getValueOfVariable("id");

	if (id == SPACE_ASCII) {
		spaceWidth = (getValueOfVariable("xadvance") - paddingWidth) * horizontalPerPixelSize;
		return Character();
	}

	double xTex = ((double)getValueOfVariable("x") + (padding[PAD_LEFT] - DESIRED_PADDING)) / imageSize;
	double yTex = ((double)getValueOfVariable("y") + (padding[PAD_TOP] - DESIRED_PADDING)) / imageSize;

	int width = getValueOfVariable("width") - (paddingWidth - (2 * DESIRED_PADDING));
	int height = getValueOfVariable("height") - ((paddingHeight)-(2 * DESIRED_PADDING));

	double quadWidth = width * horizontalPerPixelSize;
	double quadHeight = height * verticalPerPixelSize;

	double xTexSize = (double)width / imageSize;
	double yTexSize = (double)height / imageSize;

	double xOff = (getValueOfVariable("xoffset") + padding[PAD_LEFT] - DESIRED_PADDING) * horizontalPerPixelSize;
	double yOff = (getValueOfVariable("yoffset") + (padding[PAD_TOP] - DESIRED_PADDING)) * verticalPerPixelSize;

	double xAdvance = (getValueOfVariable("xadvance") - paddingWidth) * horizontalPerPixelSize;

	return Character(id, xTex, yTex, xTexSize, yTexSize, xOff, yOff, quadWidth, quadHeight, xAdvance);
}

double MetaFile::getSpaceWidth() {
	return spaceWidth;
}

Character MetaFile::getCharacter(int ascii) {
	return metaData.find(ascii)->second;
}
