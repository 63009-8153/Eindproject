#ifndef _WE_TEXTURE_2D_
#define _WE_TEXTURE_2D_

#include "../Utils/loader.h"

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

		glm::vec4 getPixelValue(int x, int y);

		GLuint textureid;
		int width,
			height,
			filesize;

		glm::vec2 position, scale;
		float rotation;

		GLenum imageType;

		unsigned char * data; // Actual RGBA data buffer
		unsigned int dataSize;

	private:
		
};

#endif