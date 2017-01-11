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
Word::Word(double fontSize) {
	fontSize = fontSize;
}

/**
* Adds a character to the end of the current word and increases the screen-space width of the word.
* @param character - the character to be added.
*/
void Word::addCharacter(Character character) {
	characters.add(character);
	width += character.getxAdvance() * fontSize;
}

/**
* @return The list of characters in the word.
*/
List<Character> Word::getCharacters() {
	return characters;
}

/**
* @return The width of the word in terms of screen size.
*/
double Word::getWordWidth() {
	return width;
}