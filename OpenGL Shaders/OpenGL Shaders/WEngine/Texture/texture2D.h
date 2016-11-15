#ifndef _WE_TEXTURE_2D_
#define _WE_TEXTURE_2D_

#include "../Utils/loader.h"

class texture2D
{
	public:
		texture2D();
		~texture2D();

		void loadImage(const char * filename, bool generateMipmaps);
		void loadImage(const char * filename);

		glm::vec4 getPixelValue(int x, int y);

		GLuint textureid;
		int width,
			height,
			filesize;

		GLenum imageType;

		unsigned char * data; // Actual RGBA data buffer
		unsigned int dataSize;

	private:
		Loader loader;
};

#endif