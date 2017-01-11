#include "MetaFile.h"

MetaFile::MetaFile()
{
}

MetaFile::~MetaFile()
{
}

/**
* Read in the next line and store the variable values.
*
* @return {@code true} if the end of the file hasn't been reached.
*/
bool MetaFile::processNextLine() {
	values.clear();
	String line = null;
	try {
		line = reader.readLine();
	}
	catch (IOException e1) {
	}
	if (line == null) {
		return false;
	}
	for (String part : line.split(SPLITTER)) {
		String[] valuePairs = part.split("=");
		if (valuePairs.length == 2) {
			values.put(valuePairs[0], valuePairs[1]);
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
int MetaFile::getValueOfVariable(String variable) {
	return Integer.parseInt(values.get(variable));
}

/**
* Gets the array of ints associated with a variable on the current line.
*
* @param variable
*            - the name of the variable.
* @return The int array of values associated with the variable.
*/
int[] MetaFile::getValuesOfVariable(String variable) {
	String[] numbers = values.get(variable).split(NUMBER_SEPARATOR);
	int[] actualValues = new int[numbers.length];
	for (int i = 0; i < actualValues.length; i++) {
		actualValues[i] = Integer.parseInt(numbers[i]);
	}
	return actualValues;
}

/**
* Closes the font file after finishing reading.
*/
void MetaFile::close() {
	try {
		reader.close();
	}
	catch (IOException e) {
		e.printStackTrace();
	}
}

/**
* Opens the font file, ready for reading.
*
* @param file
*            - the font file.
*/
void MetaFile::openFile(File file) {
	try {
		reader = new BufferedReader(new FileReader(file));
	}
	catch (Exception e) {
		e.printStackTrace();
		System.err.println("Couldn't read font meta file!");
	}
}

/**
* Loads the data about how much padding is used around each character in
* the texture atlas.
*/
void MetaFile::loadPaddingData() {
	processNextLine();
	this.padding = getValuesOfVariable("padding");
	this.paddingWidth = padding[PAD_LEFT] + padding[PAD_RIGHT];
	this.paddingHeight = padding[PAD_TOP] + padding[PAD_BOTTOM];
}

/**
* Loads information about the line height for this font in pixels, and uses
* this as a way to find the conversion rate between pixels in the texture
* atlas and screen-space.
*/
void MetaFile::loadLineSizes() {
	processNextLine();
	int lineHeightPixels = getValueOfVariable("lineHeight") - paddingHeight;
	verticalPerPixelSize = TextMeshCreator.LINE_HEIGHT / (double)lineHeightPixels;
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
		if (c != null) {
			metaData.put(c.getId(), c);
		}
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
	if (id == TextMeshCreator.SPACE_ASCII) {
		this.spaceWidth = (getValueOfVariable("xadvance") - paddingWidth) * horizontalPerPixelSize;
		return null;
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
	return new Character(id, xTex, yTex, xTexSize, yTexSize, xOff, yOff, quadWidth, quadHeight, xAdvance);
}

/**
* Opens a font file in preparation for reading.
*
* @param file
*            - the font file.
*/
MetaFile::MetaFile(File file) {
	this.aspectRatio = (double)Display.getWidth() / (double)Display.getHeight();
	openFile(file);
	loadPaddingData();
	loadLineSizes();
	int imageWidth = getValueOfVariable("scaleW");
	loadCharacterData(imageWidth);
	close();
}

double MetaFile::getSpaceWidth() {
	return spaceWidth;
}

Character MetaFile::getCharacter(int ascii) {
	return metaData.get(ascii);
}
