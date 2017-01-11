#include "Word.h"

Word::Word()
{
}

Word::~Word()
{
}

/**
* Create a new empty word.
* @param fontSize - the font size of the text which this word is in.
*/
Word::Word(double _fontSize) {
	fontSize = _fontSize;
}

/**
* Adds a character to the end of the current word and increases the screen-space width of the word.
* @param character - the character to be added.
*/
void Word::addCharacter(Character &character) {
	characters.push_back(character);
	width += character.getxAdvance() * fontSize;
}

/**
* @return The list of characters in the word.
*/
std::vector<Character> Word::getCharacters() {
	return characters;
}

/**
* @return The width of the word in terms of screen size.
*/
double Word::getWordWidth() {
	return width;
}