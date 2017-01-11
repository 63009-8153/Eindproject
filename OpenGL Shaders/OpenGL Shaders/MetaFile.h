#pragma once

#define PAD_TOP = 0;
#define PAD_LEFT = 1;
#define PAD_BOTTOM = 2;
#define PAD_RIGHT = 3;

#define DESIRED_PADDING = 3;

#define SPLITTER = " ";
#define NUMBER_SEPARATOR = ",";

class MetaFile
{
public:
	MetaFile();
	~MetaFile();

private:
	double aspectRatio;

	double verticalPerPixelSize;
	double horizontalPerPixelSize;
	double spaceWidth;
	int[] padding;
	int paddingWidth;
	int paddingHeight;

	Map<Integer, Character> metaData = new HashMap<Integer, Character>();

	BufferedReader reader;
	Map<String, String> values = new HashMap<String, String>();

	/**
	* Read in the next line and store the variable values.
	*
	* @return {@code true} if the end of the file hasn't been reached.
	*/
	bool processNextLine();

	/**
	* Gets the {@code int} value of the variable with a certain name on the
	* current line.
	*
	* @param variable
	*            - the name of the variable.
	* @return The value of the variable.
	*/
	int getValueOfVariable(String variable);

	/**
	* Gets the array of ints associated with a variable on the current line.
	*
	* @param variable
	*            - the name of the variable.
	* @return The int array of values associated with the variable.
	*/
	int[] getValuesOfVariable(String variable);

	/**
	* Closes the font file after finishing reading.
	*/
	void close();

	/**
	* Opens the font file, ready for reading.
	*
	* @param file
	*            - the font file.
	*/
	void openFile(File file);

	/**
	* Loads the data about how much padding is used around each character in
	* the texture atlas.
	*/
	void loadPaddingData();

	/**
	* Loads information about the line height for this font in pixels, and uses
	* this as a way to find the conversion rate between pixels in the texture
	* atlas and screen-space.
	*/
	void loadLineSizes();

	/**
	* Loads in data about each character and stores the data in the
	* {@link Character} class.
	*
	* @param imageWidth
	*            - the width of the texture atlas in pixels.
	*/
	void loadCharacterData(int imageWidth);

	/**
	* Loads all the data about one character in the texture atlas and converts
	* it all from 'pixels' to 'screen-space' before storing. The effects of
	* padding are also removed from the data.
	*
	* @param imageSize
	*            - the size of the texture atlas in pixels.
	* @return The data about the character.
	*/
	Character loadCharacter(int imageSize);

protected:
	/**
	* Opens a font file in preparation for reading.
	*
	* @param file
	*            - the font file.
	*/
	MetaFile(File file);

	double getSpaceWidth();

	Character getCharacter(int ascii);
};

