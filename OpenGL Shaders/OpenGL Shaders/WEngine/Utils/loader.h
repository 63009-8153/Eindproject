#ifndef _LOADER_
#define _LOADER_

#include "File.h"
#include "../GameObject/gameobject.h"

class Loader
{
public:
	~Loader();

	gameobject loadToVAO(GLfloat positions[], int posCount, GLuint indices[], int indicesCount, GLfloat uvs[], int uvsSize, GLfloat normals[], int normalsSize);
	gameobject loadToVAO(GLfloat positions[], int posCount, GLuint indices[], int indicesCount, GLfloat uvs[], int uvsSize, GLfloat normals[], int normalsSize, GLfloat tangents[], int tangentsSize);
	gameobject loadToVAO(GLfloat positions[], int posCount, int dimensions);

	gameobject loadObjFile(const char * path, bool writeParsed, bool forceParse);

	GLuint loadTexture(const char * imagepath, bool createMipmap);

	GLuint loadTextureInData(const char * imagepath, bool bmpAlign, bool createMipmap, unsigned char *&data, int & width, int & height, int & size, GLenum & type);
	void loadTextureInData(const char * imagepath, bool bmpAlign, unsigned char *&data, int & width, int & height, int & size, GLenum & type);

	GLuint loadCubeMap(unsigned char * data[], glm::vec2 size[], GLenum type);


	GLuint createEmptyVbo(int floatCount);
	void addInstancedAttribute(GLuint vao, GLuint vbo, GLuint attribute, GLuint dataSize, GLuint instancedDataLength, GLuint offset);
	void updateVbo(GLuint vbo, GLfloat data[], int dataSize);

	void cleanUp();

private:
	std::vector<GLuint> vaos, vbos, textures;

	//std::vector<face> faces;

	GLuint loadBMP(const char * imagepath);
	GLuint loadBMPRGBA(const char * imagepath);
	GLuint loadPNG(const char * imagepath);

	GLuint createVAO();
	void storeDataInAttributeList(int attributeNumber, int coordSize, GLfloat data[], int size);
	void bindIndicesBuffer(GLuint indices[], int amount);
	void unbindVAO();

	void computeTangentBasis(
		// inputs
		glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
		glm::vec2 uv1,  glm::vec2 uv2,  glm::vec2 uv3,
		// outputs
		std::vector<glm::vec3> & tangents);

	void processVertexTangent(int index, glm::vec3 tangent, std::vector<glm::vec3> & tangents, std::vector<bool> & tangentsSet);
};

#endif
