#include "FontType.h"

FontType::FontType()
{
}

FontType::~FontType()
{
}

/**
* Creates a new font and loads up the data about each character from the
* font file.
*
* @param textureAtlas
*            - the ID of the font atlas texture.
* @param fontFile
*            - the font file containing information about each character in
*            the texture atlas.
*/
FontType::FontType(int textureAtlas, char * fontFile) {
	textureAtlas = textureAtlas;
	loader = TextMeshCreator(fontFile);
}

/**
* @return The font texture atlas.
*/
int FontType::getTextureAtlas() {
	return textureAtlas;
}

/**
* Takes in an unloaded text and calculate all of the vertices for the quads
* on which this text will be rendered. The vertex positions and texture
* coords and calculated based on the information from the font file.
*
* @param text
*            - the unloaded text.
* @return Information about the vertices of all the quads.
*/
TextMeshData FontType::loadText(GUIText text) {
	return loader.createTextMesh(text);
}
