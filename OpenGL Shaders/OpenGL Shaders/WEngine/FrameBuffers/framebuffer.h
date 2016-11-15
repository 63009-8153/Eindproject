#ifndef __FRAMEBUFFERS__
#define __FRAMEBUFFERS__

class FrameBuffer
{
	public:
	~FrameBuffer();

	static void bindFrameBuffer(GLuint framebuffer, int width, int height);
	static void unbindCurrentFrameBuffer(int DisplayWidth, int DisplayHeight);
	static void resolveToFrameBuffer(GLuint inputFrameBuffer, int iWidth, int iHeight, GLuint outputFrameBuffer, int oWidth, int oHeight);
	static void resolveToScreen(GLuint inputFrameBuffer, int iWidth, int iHeight, int screenWidth, int screenHeight);

	static GLuint createFrameBuffer();
	static GLuint createTextureAttachment(int width, int height);
	static GLuint createDepthTextureAttachment(int width, int height);
	static GLuint createDepthBufferAttachment(int width, int height);
	static GLuint createDepthBufferAttachment(int width, int height, GLenum attachmentFormat);
	static GLuint createMultisampledDepthBufferAttachment(int width, int height, int samples);
	static GLuint createMultisampledColourAttachment(int width, int height, int samples);

	static void deleteFrameBuffer(GLuint framebufferID);
	static void deleteTexture(GLuint textureID);
	static void deleteRenderBuffer(GLuint renderbufferID);

	
	private:
		
		FrameBuffer();
};

#endif