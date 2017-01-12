#pragma once

#include "Header.h"

class Character
{
	public:
		Character();
		~Character();

		Character(int id, double xTextureCoord, double yTextureCoord, double xTexSize, double yTexSize,
					double xOffset, double yOffset, double sizeX, double sizeY, double xAdvance);

		int getId();

		double getxTextureCoord();

		double getyTextureCoord();

		double getXMaxTextureCoord();

		double getYMaxTextureCoord();

		double getxOffset();

		double getyOffset();

		double getSizeX();

		double getSizeY();

		double getxAdvance();

	private:

		/**
		* @param id
		*            - the ASCII value of the character.
		* @param xTextureCoord
		*            - the x texture coordinate for the top left corner of the
		*            character in the texture atlas.
		* @param yTextureCoord
		*            - the y texture coordinate for the top left corner of the
		*            character in the texture atlas.
		* @param xTexSize
		*            - the width of the character in the texture atlas.
		* @param yTexSize
		*            - the height of the character in the texture atlas.
		* @param xOffset
		*            - the x distance from the curser to the left edge of the
		*            character's quad.
		* @param yOffset
		*            - the y distance from the curser to the top edge of the
		*            character's quad.
		* @param sizeX
		*            - the width of the character's quad in screen space.
		* @param sizeY
		*            - the height of the character's quad in screen space.
		* @param xAdvance
		*            - how far in pixels the cursor should advance after adding
		*            this character.
		*/

		int id;

		double xTextureCoord,
			yTextureCoord,
			xMaxTextureCoord,
			yMaxTextureCoord,
			xOffset,
			yOffset,
			sizeX,
			sizeY,
			xAdvance;

};

