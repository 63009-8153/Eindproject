#pragma once

#include "../../Header.h"

class TextMeshData;
#include "TextMeshData.h"

class GUIText;
#include "GUIText.h"

class TextMeshCreator;
#include "TextMeshCreator.h"

/**
 * Represents a font. It holds the font's texture atlas as well as having the
 * ability to create the quad vertices for any text using this font.
 */
class FontType
{
	public:
		FontType();
		~FontType();

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
		FontType(int textureAtlas, char * fontFile);

		/**
		* @return The font texture atlas.
		*/
		int getTextureAtlas();

		/**
		* Takes in an unloaded text and calculate all of the vertices for the quads
		* on which this text will be rendered. The vertex positions and texture
		* coords and calculated based on the information from the font file.
		*
		* @param text
		*            - the unloaded text.
		* @return Information about the vertices of all the quads.
		*/
		TextMeshData loadText(GUIText &text);

	private:

		int textureAtlas;
		TextMeshCreator *loader;
};

