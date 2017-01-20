#ifndef _WE_TEXTURE_2D_
#define _WE_TEXTURE_2D_

#include "../Utils/loader.h"
#include "../Renderer/GuiShaderProgram.h"

extern Loader loader;

class texture2D
{
	public:
		texture2D();
		~texture2D();

		//Load texture data and create texture
		void loadImage(const char * filename, bool bmpAlign, bool generateMipmaps);
		//Only load texture data
		void loadImage(const char * filename, bool bmpAlign);
		//Only load image and creating texture
		void loadImage(const char * filename);

		void Draw(GuiShaderProgram &shader, gameobject &quad);

		glm::vec4 getPixelValue(int x, int y);

		// Set the position. This will convert 0,0 upper left corner to -1,-1 for proper display.
		void setPosition(glm::vec2 pos);
		// Get the position
		glm::vec2 getPosition();

		// Set the rotation in degress
		void setRotation(float rot);
		// Get the rotation in radians
		float getRotationRad();
		// Get the rotation in degrees
		float getRotationDeg();

		// Set the scale
		void setScale(glm::vec2 scal);
		// Get the scale
		glm::vec2 getScale();

		GLuint textureid;
		int width,
			height,
			filesize;

		GLenum imageType;

		unsigned char * data; // Actual RGBA data buffer
		unsigned int dataSize;

	private:
		glm::vec2 position, scale;
		float rotation;
};

#endif