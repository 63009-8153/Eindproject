#ifndef _WE_TEXTURE_CUBEMAP_
#define _WE_TEXTURE_CUBEMAP_

#include "texture2D.h"

#include "../Utils/loader.h"

class textureCubemap
{
	public:
		textureCubemap();
		~textureCubemap();

		void initialseFrameBuffer(int size);

		void createEmptyCubeMap();
		void loadCubeMap(std::string filename[6]);

		void bindFrameBuffer();
		void bindFrameBufferRenderTexture(GLenum side);
		void unbindFrameBuffer(int sw, int sh);
		void deleteBuffers();

		GLuint textureid;

		texture2D textures[6];

	private:
		Loader loader;

		int textureSize;
		bool initialisedForFrameBuffer;
		GLuint frameBufferId,
			depthBufferId;
};

#endif